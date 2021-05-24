#include <sstream>
#include <dirent.h>
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"
#include "../semantic/GlobalProcessor.h"
#include "../semantic/Checker.h"
#include "../transpiler/STranspiler.h"
#include <unistd.h>

#define REQUIREMENTS_FILE "requirements.txt"
static std::string lib_path;
static bool global_fail = false;
static std::map<std::string, std::string> function_builtins;
static std::map<std::string, std::map<std::string, std::string>> class_builtins;
static Package* root_package;
static std::string static_initializations;
static std::string static_cleanups;

static Package* top_package;

BlockNode* full_parse(const std::string& __file__, CodeLines* code_lines) {
    BlockNode* tree;
    Scanner scanner;
    scanner.load_file(__file__);
    std::vector<Token> tokens = scanner.scan_all();
    // for (auto token: tokens) {
    //     std::cout << token.to_string() << std::endl;
    // }
    Parser parser(__file__, scanner.code_lines, tokens);
    *code_lines = scanner.code_lines;
    try {
        tree = parser.parse_program();
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        exit(0);
    }
    return tree;
}

VectorOfStrings all_modules;

void load_package(Package* package, int level) {
    DIR* dir = opendir(package->abs_path.c_str());
    if (dir == nullptr) {
        std::cerr << "No such dir for package '" << package->name << "': " << "'" << package->abs_path << "'"
                  << std::endl;
        return;
    }
    dirent* ent = readdir(dir);
    while (ent != nullptr) {
        std::string d_name = ent->d_name;
        if (d_name != "." && d_name != "..") {
            if (ent->d_type == DT_REG) {
                std::string file_extension = d_name.substr(d_name.size() - 3, 3);
                if (file_extension == ".xl") {
                    std::string module_abs_path = path_join(package->abs_path, d_name);
                    std::string module_rel_path = path_join(package->rel_path, d_name);
                    all_modules.push_back(module_rel_path);
                    std::string module_name = d_name.substr(0, d_name.size() - 3);
                    auto* module = new Module(Path(package->path, module_name), module_abs_path, module_rel_path);
                    package->units[module_name] = Unit{.type=U_TYPE::MODULE, .module=module};
                } else {
                    // std::cout << std::string(level + 1, '\t') << "OTHER: " << d_name << std::endl;
                }
            } else if (ent->d_type == DT_DIR) {
                std::string subpackage_abs_path = path_join(package->abs_path, d_name);
                std::string subpackage_rel_path = path_join(package->rel_path, d_name);
                const std::string& subpackage_name = d_name;
                auto* subpackage = new Package(Path(package->path, d_name),
                                                  subpackage_abs_path,
                                                  subpackage_rel_path);
                load_package(subpackage, level + 1);
                package->units[subpackage_name] = Unit{.type=U_TYPE::PACKAGE, .package=subpackage};
            }
        }
        ent = readdir(dir);
    }
}

void parse_all_modules(Package* package) {
    if (package->units.empty()) {
        std::cerr << "Warning: package " << package->name << " (" << package->abs_path << ") is empty" << std::endl;
        return;
    }
    for (const auto& ep: package->units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            parse_all_modules((Package*) subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            CodeLines code_lines;
            BlockNode* ast = full_parse(module->abs_path, &code_lines);
            module->ast = ast;
            module->code_lines = code_lines;
        }
    }
}

void process_global_all_modules(Package* package) {
    for (const auto& ep: package->units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            process_global_all_modules(subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            GlobalProcessor gp;
            gp.module = module;
            gp.__file__ = module->abs_path;
            std::cout << "Global-processing module " << module->name << " at path: " << module->abs_path << std::endl;
            gp.visit_root(*module->ast);
        }
    }
}

VectorOfStrings make_path(const std::string& s) {
    VectorOfStrings path;
    path.push_back("");
    for (char c : s) {
        if (c != '.') {
            path.back() += c;
        } else {
            path.push_back("");
        }
    }
    return path;
}

void add_local_path_to_module(Module& module, Path path) {
    auto current_flirpin = Flirpin{.type=F_TYPE::PACKAGE, .package=top_package};
    std::string path_so_far = "global";
    std::string last_include;
    Flirpin last_flirpin;
    for (const auto& path_part: path.as_vec()) {
        if (current_flirpin.type == F_TYPE::PACKAGE) {
            Package* package = current_flirpin.package;
            auto unit = package->units.find(path_part);
            if (unit == package->units.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in package '" + path_so_far + "'");
            }
            if (unit->second.type == U_TYPE::MODULE) {
                last_include = unit->second.module->rel_path;
            } else {
                last_include = unit->second.package->rel_path;
            }

            current_flirpin = map_unit_to_flirpin(unit->second);
            last_flirpin = current_flirpin;
        } else if (current_flirpin.type == F_TYPE::MODULE) {
            auto flirpin = current_flirpin.module->flirpins.find(path_part);
            if (flirpin == current_flirpin.module->flirpins.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in module '" + path_so_far + "'");
            }
            current_flirpin = flirpin->second;
        }
        path_so_far += "." + path_part;
    }
    // std::string t = flirpintype_to_str(current_flirpin.type);
    // std::string lt = flirpintype_to_str(last_flirpin.type);
    std::string included_header;
    if (last_flirpin.type == F_TYPE::PACKAGE) {
        included_header = last_flirpin.package->rel_path + "/__package__";
        module.included_module_paths.push_back(included_header + ".h");
        module.flirpins[path.as_vec().back()] = current_flirpin;
    } else {
        included_header = last_include.substr(0, last_include.size() - 3);
        module.included_module_paths.push_back(included_header + ".h");
        module.flirpins[path.as_vec().back()] = current_flirpin;
    }
}

void add_global_path_to_module(Module* module, Path path) {

}

void add_path_to_module(Module& module, Path path) {
    // std::cout << "######## adding path: " << path.as_str() << std::endl;
    VectorOfStrings pv = path.as_vec();
    if (pv[0] == "root") {
        VectorOfStrings path_vec = VectorOfStrings(pv.begin(), pv.end());
        add_local_path_to_module(module, Path(path_vec));
    } else {
        VectorOfStrings path_vec = VectorOfStrings(pv.begin(), pv.end());
        add_local_path_to_module(module, Path(path_vec));
        // add_global_path_to_module(module, path);
    }
}

void add_path_with_alias_to_module(Module& module, const std::string& alias, Path path) {
    auto current_flirpin = Flirpin{.type=F_TYPE::PACKAGE, .package=root_package};
    std::string path_so_far;
    std::string last_include;
    for (const auto& path_part: path.as_vec()) {
        if (current_flirpin.type == F_TYPE::PACKAGE) {
            Package* package = current_flirpin.package;
            auto unit = package->units.find(path_part);
            if (unit == package->units.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in package " + path_so_far);
            }
            if (unit->second.type == U_TYPE::MODULE) {
                last_include = unit->second.module->rel_path;
            } else {
                last_include = unit->second.package->rel_path;
            }
            current_flirpin = map_unit_to_flirpin(unit->second);
        } else if (current_flirpin.type == F_TYPE::MODULE) {
            auto flirpin = current_flirpin.module->flirpins.find(path_part);
            if (flirpin == current_flirpin.module->flirpins.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in module " + path_so_far);
            }
            current_flirpin = flirpin->second;
        }
        path_so_far += "." + path_part;
    }
    if (current_flirpin.type == F_TYPE::CLASS) {
        throw std::runtime_error(
                "Cannot import class " + current_flirpin.clazz->path.as_str() + " aliased with " + alias);
    }
    std::string included_module_header_basename = last_include.substr(0, last_include.size() - 3);
    module.included_module_paths.push_back(included_module_header_basename + ".h");
    module.flirpins[alias] = current_flirpin;
}

std::string project_output_dir;

void analyze_module(Module& module) {
    // if (module.name == "core") {
    //     return;
    // }

    for (const auto& path: module.imported_paths_no_alias_v) {
        // std::cout << "PATH: " << path.first << std::endl;
        add_path_to_module(module, path.second);
    }
    for (const auto& i: module.imported_paths_with_alias_v) {
        // std::cout << "PATH: " << i.first << std::endl;
        add_path_with_alias_to_module(module, i.first, i.second);
    }
    Checker checker;
    checker.top_package = top_package;
    checker.module = &module;
    checker.__file__ = module.abs_path;
    checker.code_lines = module.code_lines;
    checker.visit_root(*module.ast);
    module.sast = checker.root_snode;
    if (checker.error_reporter.failed) {
        global_fail = true;
        // throw std::runtime_error("Semantic analysis failed for module " + module.abs_path);
    }
}

void analyze_all_modules(Package& package) {
    // std::cout << "Analyzing package " << package->name << std::endl;
    for (const auto& ep: package.units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package& subpackage = *ep.second.package;
            analyze_all_modules(subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module& module = *ep.second.module;
            analyze_module(module);
        }
    }
}

void transpile_one_module(Module& module, std::string& package_header, const std::string& output_package_dir,
                          Package& package) {
    if (module.name == "core") {
        return;
    }
    STranspiler t;
    t.transpile_program(module.sast);
    // std::cout << "Source output: " << std::endl;
    // std::cout << t.source << std::endl;
    // std::cout << "Header output: " << std::endl;
    // std::cout << t.header << std::endl;
    // std::cout << "- Done" << std::endl;

    static_initializations += t.static_initializations;
    static_cleanups += t.static_cleanups;

    std::string module_name = module.name;

    if (module.flirpins.count("main") != 0) {
        t.source += "\nint main(){\n";
        t.source += static_initializations;
        t.source += "auto x = GET_INT(CALL0(" + mangle_path(module.path.as_str() + ".main") + "));\n";
        t.source += static_cleanups;
        t.source += "return x;\n}";
    }
    std::string output_cpp_path = path_join(output_package_dir, module_name + ".cpp");
    std::ofstream output_cpp_file(output_cpp_path);
    output_cpp_file << "#include \"" << module_name << ".h\"\n";
    output_cpp_file << t.source;

    std::string output_h_path = path_join(output_package_dir, module_name + ".h");


    std::string module_define = module_name + "_H";
    std::string h_ifndef = "#ifndef " + module_define + "\n";
    std::string h_define = "#define " + module_define + "\n";
    std::string h_endif = "#endif //" + module_define + "\n";
    std::string include_core = "#include <core/core.h>\n";
    std::string includes = include_core;
    for (const auto& m: module.included_module_paths) {
        if (m == "core.h") {
            continue;
        }
        includes += "#include <" + m + ">\n";
    }
    t.header = includes + t.header;

    t.header = h_ifndef + h_define + t.header + h_endif;
    std::ofstream output_h_file(output_h_path);
    output_h_file << t.header;
    package_header += "#include <" + package.rel_path + "/" + module_name + ".h>\n";
}

std::map<std::string, std::string> read_requirements(const std::string& filepath) {
    std::string line;
    std::map<std::string, std::string> requirements;
    std::ifstream infile(filepath);
    if (!infile.is_open()) {
        std::cerr << "No requirements file at " << filepath << std::endl;
        return requirements;
    }
    while (std::getline(infile, line)) {
        std::string op;
        std::string first;
        std::string second;
        for (char i : line) {
            if (i == '=') {
                op += '=';
            } else {
                if (op == "") {
                    first += i;
                } else {
                    second += i;
                }
            }
        }
        requirements[first] = second;
    }
    return requirements;
}

std::map<std::string, bool> loaded_top_units;

void load_top_unit(const std::string& name, const std::string& top_unit_path) {
    if (loaded_top_units.count(top_unit_path) != 0) {
        // skip, already loaded
        return;
    }
    std::cout << "Loading top unit: " << E_HLT(name) << " at path: " << E_HLT(top_unit_path) << std::endl;
    std::string unit_requirements_file = path_join(top_unit_path, REQUIREMENTS_FILE);
    auto requirements = read_requirements(unit_requirements_file);
    for (const auto& req: requirements) {
        std::string req_top_unit_path = path_join(path_join(lib_path, req.first), req.second);
        load_top_unit(req.first, req_top_unit_path);
    }

    auto* top_unit_package = new Package(Path(name), path_join(top_unit_path, "src"), "");
    load_package(top_unit_package, 0);
    parse_all_modules(top_unit_package);
    process_global_all_modules(top_unit_package);
    top_package->units[name] = Unit{.type=U_TYPE::PACKAGE, .package=top_unit_package};
    std::cout << "Finished loading top unit: " << E_HLT(top_unit_path) << std::endl;
    loaded_top_units[top_unit_path] = true;
}

void load_requirements(const std::string& filepath) {
    auto requirements = read_requirements(filepath);

    bool has_error = false;
    for (const auto& r: requirements) {
        std::string final_path = path_join(path_join(lib_path, r.first), r.second);
        DIR* dir = opendir(final_path.c_str());
        if (dir == nullptr) {
            std::cout << "REQUIREMENT " << r.first + "==" + r.second << " NOT FOUND" << std::endl;
            has_error = true;
            continue;
        }
        load_top_unit(r.first, final_path);
        // Package* req_package = new Package(r.first, final_path, "");
        // load_package(req_package, 0);
        // std::cout << "LOADED REQUIREMENT " << r.first + "==" + r.second << std::endl;
    }
    if (has_error) {
        std::cerr << "Error loading requirements" << std::endl;
        exit(1);
    }
}

const int NEW_DIR_MODE = 0777;

void transpile_all_modules(Package& package, const std::string& output_dir, bool is_top) {
    // std::cout << "Transpiling package " << package.name << " output dir: " << output_dir << std::endl;
    std::string package_header;
    std::string output_package_dir = path_join(output_dir, package.name);
    // std::cout << "output package dir: " << output_package_dir << std::endl;
    if (package.name != "") {
        mkdir(output_package_dir.c_str(), NEW_DIR_MODE);
    }
    Module* main_module = nullptr;
    for (const auto& u: package.units) {
        if (u.second.type == U_TYPE::PACKAGE) {
            Package& subpackage = *u.second.package;
            transpile_all_modules(subpackage, output_package_dir, false);
            package_header += "#include <" + subpackage.rel_path + "/__package__.h>\n";
        } else if (u.second.type == U_TYPE::MODULE) {
            Module& module = *u.second.module;
            if (module.flirpins.count("main") != 0) {
                main_module = &module;
                continue;
            }
            transpile_one_module(module, package_header, output_package_dir, package);
        }
    }
    if (is_top && main_module != nullptr) {
        transpile_one_module(*main_module, package_header, output_package_dir, package);
    }
    std::string output_package_header_path = path_join(output_package_dir, "__package__.h");
    std::ofstream output_package_header(output_package_header_path);
    // std::cout << "PACKAGE HEADER: " << package.path.as_str() << std::endl << package_header;
    output_package_header << package_header;
}


int main(int argc, char* argv[]) {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    std::cout << "working directory: " << cwd << std::endl;
    if (argc < 3) {
        std::cout << style(RED, "Error: expected 2 args: source_dir output_dir") << std::endl;
        exit(0);
    }
    lib_path = getenv("XLPATH");
    std::string project_dir = argv[1];
    std::string top_project_dir = argv[1];
    project_dir = path_join(project_dir, "src");
    project_output_dir = argv[2];
    project_output_dir += "/application";
    // int x = mkdir(project_output_dir.c_str(), 0700);
    // if (x != 0) {
    //     std::cout << "failed to create  application dir" << std::endl;
    //     exit(0);
    // }
    std::string __main_file__ = path_join(project_dir, u_basename(project_dir) + ".xl");
    // std::cout << style(BLUE, "Main file: ") << style(MAGENTA, __main_file__) << std::endl;
    root_package = new Package(Path("root"), project_dir, "");

    std::string req_file_path = path_join(top_project_dir, REQUIREMENTS_FILE);
    top_package = new Package(Path("global"), "", "");
    top_package->units["root"] = Unit{.type=U_TYPE::PACKAGE, .package=root_package};
    load_requirements(req_file_path);
    load_package(root_package, 0);
    parse_all_modules(root_package);

    process_global_all_modules(root_package);

    std::cout << "here" << std::endl;
    try {
        analyze_all_modules(*root_package);
    } catch (const std::runtime_error& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        exit(0);
    }
    if (global_fail) {
        std::cout << "Failed to compile" << std::endl;
        exit(0);
    }

    std::cout << "here" << std::endl;

    transpile_all_modules(*root_package, project_output_dir, true);

    std::cout << "here" << std::endl;


    std::string all_files;
    for (auto f: all_modules) {
        if (f == "core.xl") {
            continue;
        }
        f = f.substr(0, f.size() - 3) + ".cpp";
        // std::cout << f << std::endl;
        all_files += f + " ";
    }

    std::string cmakelists = "cmake_minimum_required(VERSION 3.16)\n"
                             "project(xlang)\n"
                             "set(CMAKE_CXX_STANDARD 14)\n"
                             "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -Werror -O0 -fverbose-asm -Winline\")\n";
    cmakelists = "cmake_minimum_required(VERSION 3.16)\n"
                 "project(xlang)\n"
                 "set(CMAKE_CXX_STANDARD 14)\n"
                 "include_directories(.)\n"
                 "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -Werror -O0 -fverbose-asm -Winline\")\n";
    cmakelists += "add_executable(result " + all_files + ")\n";
    cmakelists += "target_link_libraries(result core)\n";
    std::string cmake_output = project_output_dir + "/CMakeLists.txt";
    std::ofstream cmakelists_file(cmake_output);
    std::cout << cmake_output << std::endl;
    cmakelists_file << cmakelists;

    std::string command = "cp -r /home/chris/CLionProjects/compiler/runtime " + std::string(argv[2]) + "/runtime";
    system(command.c_str());

    std::string top_level_cmake = "cmake_minimum_required(VERSION 3.16)\n"
                                  "project(xlang)\n"
                                  "set(CMAKE_CXX_STANDARD 14)\n"
                                  "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -Werror -O0 -fverbose-asm -Winline\")\n"
                                  "include_directories(/home/chris/CLionProjects/compiler/runtime)\n"
                                  "link_directories(/home/chris/CLionProjects/compiler/runtime/build)\n"
                                  "add_subdirectory(runtime)\n"
                                  "add_subdirectory(application)\n"
                                  "target_link_libraries(result core)\n";

    std::string top_cmake_output = std::string(argv[2]) + "/CMakeLists.txt";
    std::ofstream top_cmakelists_file(top_cmake_output);
    top_cmakelists_file << top_level_cmake;
    std::cout << "top cmake output: " << top_cmake_output << std::endl;
}