#include <sstream>
#include <dirent.h>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "semantic/GlobalProcessor.h"
#include "semantic/Checker.h"
#include "logging/logging.h"
#include "transpiler/STranspiler.h"
#include "units/Package.h"

static bool global_fail = false;
static std::map<std::string, std::string> function_builtins;
static std::map<std::string, std::map<std::string, std::string>> class_builtins;
static Package* root_package;
static std::string static_initializations;
static std::string static_cleanups;

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
        // std::cout << e.what() << std::endl;
        exit(1);
    }
    return tree;
}

VectorOfStrings all_modules;

void build_packages(Package* package, int level) {
    DIR* dir = opendir(package->abs_path.c_str());
    // std::cout << std::string(level, '\t') << "PACKAGE: " << package->abs_path << std::endl;
    dirent* ent = readdir(dir);
    while (ent != nullptr) {
        std::string d_name = ent->d_name;
        if (d_name != "." && d_name != "..") {
            if (ent->d_type == DT_REG) {
                std::string file_extension = d_name.substr(d_name.size() - 3, 3);
                if (file_extension == ".xl") {
                    // std::cout << std::string(level + 1, '\t') << "MODULE: " << d_name << std::endl;
                    std::string module_abs_path = path_join(package->abs_path, d_name);
                    std::string module_rel_path = path_join(package->rel_path, d_name);
                    all_modules.push_back(module_rel_path);
                    std::string module_name = d_name.substr(0, d_name.size() - 3);
                    Module* module = new Module(module_name, module_abs_path, module_rel_path);
                    module->path = Path(package->path, module_name);
                    package->units[module_name] = Unit{.type=U_TYPE::MODULE, .module=module};
                } else {
                    // std::cout << std::string(level + 1, '\t') << "OTHER: " << d_name << std::endl;
                }
            } else if (ent->d_type == DT_DIR) {
                std::string subpackage_abs_path = path_join(package->abs_path, d_name);
                std::string subpackage_rel_path = path_join(package->rel_path, d_name);
                std::string subpackage_name = d_name;
                Package* subpackage = new Package(d_name, subpackage_abs_path, subpackage_rel_path);
                subpackage->path = Path(package->path, d_name);
                build_packages(subpackage, level + 1);
                package->units[subpackage_name] = Unit{.type=U_TYPE::PACKAGE, .package=subpackage};
            }
        }
        ent = readdir(dir);
    }
}

void parse_all_modules(Package* package) {
    // std::cout << "Parsing package " << package->name << std::endl;
    for (auto ep: package->units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            parse_all_modules((Package*) subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            CodeLines code_lines;
            // std::cout << "Parsing module " << module->name << std::endl;
            BlockNode* ast = full_parse(module->abs_path, &code_lines);
            // std::cout << "- Done" << std::endl;
            ((Module*) module)->ast = ast;
            ((Module*) module)->code_lines = code_lines;
        }
    }
}

void process_global_all_modules(Package* package) {
    // std::cout << "Global processing package " << package->name << std::endl;
    for (auto ep: package->units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            process_global_all_modules(subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            // if (module->name == "core") {
            //     continue;
            // }
            // std::cout << "Global processing module " << module->name << std::endl;
            GlobalProcessor gp;
            // std::cout << "Global processing module " << module->name << std::endl;
            // Module* core_module = root_package->units["core"].module;
            // for (auto builtin: core_module->flirpins) {
            //     module->flirpins[builtin.first] = builtin.second;
            // }
            gp.module = module;
            gp.__file__ = module->abs_path;
            gp.visit_root(*module->ast);
            // module->imported_paths_no_alias = gp.imported_paths_no_alias;
            // module->imported_paths_with_alias = gp.imported_paths_with_alias;
            // std::cout << "- Done" << std::endl;
        }
    }
}

VectorOfStrings make_path(std::string s) {
    VectorOfStrings path;
    path.push_back("");
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] != '.') {
            path.back() += s[i];
        } else {
            path.push_back("");
        }
    }
    return path;
}

void add_path_to_module(Module* module, Path path) {
    // std::cout << "######## adding path: " << path.as_str() << std::endl;
    Flirpin current_flirpin = Flirpin{.type=F_TYPE::PACKAGE, .package=root_package};
    std::string path_so_far;
    std::string last_include;
    Flirpin last_flirpin;
    for (auto path_part: path.as_vec()) {
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
            last_flirpin = current_flirpin;
        } else if (current_flirpin.type == F_TYPE::MODULE) {
            auto flirpin = current_flirpin.module->flirpins.find(path_part);
            if (flirpin == current_flirpin.module->flirpins.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in module " + path_so_far);
            }
            current_flirpin = flirpin->second;
        }
        path_so_far += "." + path_part;
    }
    std::string t = flirpintype_to_str(current_flirpin.type);
    std::string lt = flirpintype_to_str(last_flirpin.type);
    std::string included_header;
    if (last_flirpin.type == F_TYPE::PACKAGE) {
        included_header = last_flirpin.package->rel_path + "/__package__";
        module->included_module_paths.push_back(included_header + ".h");
        module->flirpins[path.as_vec().back()] = current_flirpin;
    } else {
        included_header = last_include.substr(0, last_include.size() - 3);
        module->included_module_paths.push_back(included_header + ".h");
        module->flirpins[path.as_vec().back()] = current_flirpin;
    }

    // std::cout << "CURRENT FLIRPIN TYPE: " << t << std::endl;
    // std::cout << "last FLIRPIN TYPE: " << lt << std::endl;


    // std::cout << "######## added path: " << included_header << std::endl;
}

void add_path_with_alias_to_module(Module* module, std::string alias, Path path) {
    Flirpin current_flirpin = Flirpin{.type=F_TYPE::PACKAGE, .package=root_package};
    std::string path_so_far;
    std::string last_include;
    for (auto path_part: path.as_vec()) {
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
    module->included_module_paths.push_back(included_module_header_basename + ".h");
    module->flirpins[alias] = current_flirpin;
}

std::string project_output_dir;

void analyze_all_modules(Package* package) {
    // std::cout << "Analyzing package " << package->name << std::endl;
    for (auto ep: package->units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            analyze_all_modules(subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            if (module->name == "core") {
                continue;
            }

            for (auto path: module->imported_paths_no_alias_v) {
                // std::cout << "PATH: " << path.first << std::endl;
                add_path_to_module(module, path.second);
            }
            for (auto i: module->imported_paths_with_alias_v) {
                // std::cout << "PATH: " << i.first << std::endl;
                add_path_with_alias_to_module(module, i.first, i.second);
            }
            Checker checker;
            checker.root_package = root_package;
            checker.module = module;
            checker.__file__ = module->abs_path;
            checker.code_lines = module->code_lines;
            checker.visit_root(*module->ast);
            module->sast = checker.root_snode;
            if (checker.error_reporter.failed) {
                global_fail = true;
                // throw std::runtime_error("Semantic analysis failed for module " + module->abs_path);
            }
            // std::cout << "- Done" << std::endl;
        }
    }
}

void transpile_all_modules(Package* package, std::string output_dir) {
    // std::cout << "Transpiling package " << package->name << " output dir: " << output_dir << std::endl;
    std::string package_header;
    std::string output_package_dir = path_join(output_dir, package->name);
    // std::cout << "output package dir: " << output_package_dir << std::endl;
    if (package->name != "") {
        mkdir(output_package_dir.c_str(), 0777);
    }
    for (auto u: package->units) {
        if (u.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = u.second.package;
            transpile_all_modules(subpackage, output_package_dir);
            package_header += "#include <" + subpackage->rel_path + "/__package__.h>\n";
        } else if (u.second.type == U_TYPE::MODULE) {
            Module* module = u.second.module;
            if (module->name == "core") {
                continue;
            }
            STranspiler t;
            t.transpile_program(module->sast);
            // std::cout << "Source output: " << std::endl;
            // std::cout << t.source << std::endl;
            // std::cout << "Header output: " << std::endl;
            // std::cout << t.header << std::endl;
            // std::cout << "- Done" << std::endl;

            static_initializations += t.static_initializations;
            static_cleanups += t.static_cleanups;

            std::string module_name = module->name;

            if (module->flirpins.count("main") != 0) {
                t.source += "\nint main(){\n";
                t.source += static_initializations;
                t.source += "auto x = GET_INT(CALL0(" + mangle_path(module->path.as_str() + ".main") + "));\n";
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
            for (auto m: module->included_module_paths) {
                if (m == "core.h") {
                    continue;
                }
                includes += "#include <" + m + ">\n";
            }
            t.header = includes + t.header;

            t.header = h_ifndef + h_define + t.header + h_endif;
            std::ofstream output_h_file(output_h_path);
            output_h_file << t.header;
            package_header += "#include <" + package->rel_path + "/" + module_name + ".h>\n";
        }
    }
    std::string output_package_header_path = path_join(output_package_dir, "__package__.h");
    std::ofstream output_package_header(output_package_header_path);
    // std::cout << "PACKAGE HEADER: " << package->path.as_str() << std::endl << package_header;
    output_package_header << package_header;
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << style(RED, "Error: expected 2 args: source_dir output_dir") << std::endl;
        exit(1);
    }
    std::string project_dir = argv[1];
    project_output_dir = argv[2];
    project_output_dir += "/application";
    std::string __main_file__ = path_join(project_dir, u_basename(project_dir) + ".xl");
    // std::cout << style(BLUE, "Main file: ") << style(MAGENTA, __main_file__) << std::endl;
    root_package = new Package("", project_dir, "");
    build_packages(root_package, 0);
    parse_all_modules(root_package);

    process_global_all_modules(root_package);


    analyze_all_modules(root_package);
    if (global_fail) {
        std::cout << "Failed to compile" << std::endl;
        exit(1);
    }
    transpile_all_modules(root_package, project_output_dir);

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
    cmakelists += "add_executable(result " + all_files + ")\n";
    cmakelists += "include_directories(../runtime)\n"
                  "include_directories(.)\n"
                  "target_link_libraries(result core)\n";
    std::ofstream cmakelists_file(project_output_dir + "/CMakeLists.txt");
    cmakelists_file << cmakelists;
}