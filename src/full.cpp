#include <sstream>
#include <fstream>
#include <fmt/ostream.h>
#include <fmt/color.h>
#include <dirent.h>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "semantic/GlobalProcessor.h"
#include "semantic/Checker.h"
#include "transpiler/Transpiler.h"
#include "logging/logging.h"
#include "semantic/util.h"
#include "transpiler/STranspiler.h"
#include "units/Package.h"
#include "units/ConstFunction.h"

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
        std::cout << e.what() << std::endl;
        exit(1);
    }
    return tree;
}


void build_packages(Package* package, int level) {
    DIR* dir = opendir(package->path.c_str());
    std::cout << std::string(level, '\t') << "PACKAGE: " << package->path << std::endl;
    dirent* ent = readdir(dir);
    while (ent != nullptr) {
        std::string d_name = ent->d_name;
        if (d_name != "." && d_name != "..") {
            if (ent->d_type == DT_REG) {
                std::string file_extension = d_name.substr(d_name.size() - 3, 3);
                if (file_extension == ".xl") {
                    std::cout << std::string(level + 1, '\t') << "MODULE: " << d_name << std::endl;
                    std::string module_path = package->path + "/" + d_name;
                    std::string module_name = d_name.substr(0, d_name.size() - 3);
                    Module* module = new Module(module_name, module_path);
                    module->full_path = package->full_path + "." + d_name;
                    package->units[module_name] = Unit{.type=U_TYPE::MODULE, .module=module};
                } else {
                    std::cout << std::string(level + 1, '\t') << "OTHER: " << d_name << std::endl;
                }
            } else if (ent->d_type == DT_DIR) {
                std::string subpackage_path = package->path + "/" + d_name;
                std::string subpackage_name = d_name;
                Package* subpackage = new Package(d_name, subpackage_path);
                subpackage->full_path = package->full_path + "." + d_name;
                build_packages(subpackage, level + 1);
                package->units[subpackage_name] = Unit{.type=U_TYPE::PACKAGE, .package=subpackage};
            }
        }
        ent = readdir(dir);
    }
}

void parse_all_modules(Package* package) {
    std::cout << "Parsing package " << package->name << std::endl;
    for (auto ep: package->units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            parse_all_modules((Package*) subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            CodeLines code_lines;
            std::cout << "Parsing module " << module->name << std::endl;
            BlockNode* ast = full_parse(module->path, &code_lines);
            std::cout << "- Done" << std::endl;
            ((Module*) module)->ast = ast;
            ((Module*) module)->code_lines = code_lines;
        }
    }
}

void process_global_all_modules(Package* package) {
    std::cout << "Global processing package " << package->name << std::endl;
    for (auto ep: package->units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            process_global_all_modules(subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            std::cout << "Global processing module " << module->name << std::endl;
            GlobalProcessor gp;
            gp.module = module;
            gp.root_package = root_package;
            gp.module_dotted_path = module->full_path;
            gp.__file__ = module->path;
            gp.visit(*module->ast);
            module->imported_paths = gp.imported_paths;
            std::cout << "- Done" << std::endl;
        }
    }
}

VectorOfStrings make_path(std::string s) {
    VectorOfStrings path;
    path.push_back("");
    for (int i = 0; i < s.size(); i++) {
        if (s[i] != '.') {
            path.back() += s[i];
        } else {
            path.push_back("");
        }
    }
    return path;
}

void add_path_to_module(Module* module, std::string name, VectorOfStrings path) {
    Flirpin current_flirpin = Flirpin{.type=F_TYPE::PACKAGE, .package=root_package};
    std::string path_so_far;
    for (auto path_part: path) {
        if (current_flirpin.type == F_TYPE::PACKAGE) {
            Package* package = current_flirpin.package;
            auto unit = package->units.find(path_part);
            if (unit == package->units.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in package " + path_so_far);
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
    module->flirpins[name] = current_flirpin;
}

void analyze_all_modules(Package* package) {
    std::cout << "Analyzing package " << package->name << std::endl;
    for (auto ep: package->units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            analyze_all_modules(subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            if (module->name == "core") {
                continue;
            }

            for (auto i: module->imported_paths) {
                add_path_to_module(module, i.first, i.second);
            }

            std::cout << "Analyzing module " << module->name << std::endl;
            Module* core_module = root_package->units["core"].module;
            module->flirpins["print"] = core_module->flirpins["print"];
            module->flirpins["Integer"] = core_module->flirpins["Integer"];
            module->flirpins["String"] = core_module->flirpins["String"];
            Checker checker;
            checker.module = module;
            checker.root_package = root_package;
            checker.__file__ = module->path;
            checker.code_lines = module->code_lines;
            checker.visit_root(*module->ast);
            module->sast = checker.root_snode;
            if (checker.failed) {
                throw std::runtime_error("ERROR");
            }
            std::cout << "- Done" << std::endl;
        }
    }
}


void transpile_all_modules(Package* package, std::string output_dir) {
    std::cout << "Transpiling package " << package->name << std::endl;
    for (auto u: package->units) {
        if (u.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = u.second.package;
            transpile_all_modules(subpackage, output_dir);
        } else if (u.second.type == U_TYPE::MODULE) {
            Module* module = u.second.module;
            if (module->name == "core") {
                continue;
            }
            STranspiler t;
            t.transpile_program(module->sast);
            std::cout << "Source output: " << std::endl;
            std::cout << t.source << std::endl;
            std::cout << "Header output: " << std::endl;
            std::cout << t.header << std::endl;
            std::cout << "- Done" << std::endl;

            static_initializations += t.static_initializations;
            static_cleanups += t.static_cleanups;

            std::string module_name = module->name;

            if (module->flirpins.count("main") != 0) {
                t.source += "\nint main(){\n";
                t.source += static_initializations;
                t.source += "auto x = GET_INT(CALL0(" + mangle_path(module->full_path + ".main") + "));\n";
                t.source += static_cleanups;
                t.source += "return x;\n}";
            }
            std::string output_cpp_path = path_join(output_dir, module_name + ".cpp");
            std::ofstream output_cpp_file(output_cpp_path);
            output_cpp_file << "#include \"" << module_name << ".h\"\n";
            output_cpp_file << t.source;

            std::string output_h_path = path_join(output_dir, module_name + ".h");
            std::ofstream output_h_file(output_h_path);
            output_h_file << "#include <core/core.h>\n";
            output_h_file << t.header;
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << style(RED, "Error: expected 2 args: source_dir output_dir") << std::endl;
        exit(1);
    }
    std::string project_dir = argv[1];
    std::string output_dir = argv[2];
    std::string __main_file__ = path_join(project_dir, u_basename(project_dir) + ".xl");
    std::cout << style(BLUE, "Main file: ") << style(MAGENTA, __main_file__) << std::endl;
    root_package = new Package("", project_dir);
    build_packages(root_package, 0);
    parse_all_modules(root_package);
    process_global_all_modules(root_package);

    function_builtins["map"] = "fun(List[a],fun(a)->b)->List[b]";
    function_builtins["print"] = "fun(String)";
    function_builtins["open"] = "fun(String)->File";
    function_builtins["join"] = "fun(List[String],String)->String";
    function_builtins["range"] = "fun(Integer,Integer,Integer)->List[Integer])->String";
    function_builtins["input"] = "fun()->String";

    Module* core_module = new Module("core", "");
    core_module->full_path = "core";
    root_package->units["core"] = Unit{.type=U_TYPE::MODULE, .module=core_module};

    for (auto fb: function_builtins) {
        ConstFunction* cf = new ConstFunction();
        cf->ft = parse_function_type(fb.second);
        cf->full_path = core_module->full_path + "." + fb.first;
        core_module->flirpins[fb.first] = Flirpin{.type=F_TYPE::CONST_FUNCTION, .const_function=cf};
    }

    std::vector<Class*> cbuiltins = {make_float_class_info(), make_double_class_info(), make_file_class_info(),
                                     make_int_class_info(), make_list_class_info(), make_boolean_class_info(),
                                     make_string_class_info()};

    for (auto ci: cbuiltins) {
        ci->full_path = core_module->full_path + "." + ci->class_name;
        core_module->flirpins[ci->class_name] = Flirpin{.type=F_TYPE::CLASS, .clazz=ci};
    }

    analyze_all_modules(root_package);
    transpile_all_modules(root_package, output_dir + "/application");
}