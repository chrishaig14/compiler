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
#include "units.h"

static std::map<std::string, std::string> function_builtins;
static std::map<std::string, std::map<std::string, std::string>> class_builtins;

namespace Errors {
    void module_not_found(std::string __file__, std::string imported_module_name) {
        std::cout << style(RED, "Error in file ") << style(MAGENTA BOLD, __file__) << style(RED, ": ")
                  << style(RED, "imported module ") << style(MAGENTA BOLD, "'" + imported_module_name + "'")
                  << style(RED, " not found") << std::endl;
    }

    void
    name_not_exported_by_module(std::string __file__, std::string imported_module_name, std::string imported_name) {
        std::cout << style(RED, "Error in file ") << style(MAGENTA BOLD, __file__) << style(RED, ": ")
                  << style(RED, "imported module ") << style(MAGENTA BOLD, "'" + imported_module_name + "'")
                  << style(RED, " doesn't export name ") << style(MAGENTA BOLD, "'" + imported_name + "'") << std::endl;
    }
}

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

static std::map<std::string, FunctionTable*> module_exported_functions;
static std::map<std::string, ClassTable*> module_exported_classes;
static std::set<std::string> compiled_modules;
static std::map<std::string, std::string> builtins;
ClassTable* global_classes = new ClassTable();
FunctionTable* global_functions = new FunctionTable();

static std::map<std::string, std::unique_ptr<std::map<std::string, std::string>>> module_maps;

void full_compile(bool is_main, const std::string& __file__, const std::string& output_dir) {
    std::string module_name = module_from_path(__file__);
    std::cout << "COMPILING MODULE : " << module_name << std::endl;

    CodeLines code_lines;
    BlockNode* tree = full_parse(__file__, &code_lines);

    std::map<std::string, std::set<std::string>> imported;
    module_maps[module_name] = std::make_unique<std::map<std::string, std::string>>(std::map<std::string, std::string>());
    auto& map = *module_maps[module_name];

    for (auto builtin: builtins) {
        map[builtin.first] = builtin.second;
    }

    for (auto builtin: builtins) {
        map[builtin.first] = builtin.second;
    }

    for (auto cb: class_builtins) {
        map[cb.first] = mangle_class_name("core", cb.first);
    }

    for (auto n: tree->nodes) {
        if (n->ntype == IMPORT) {
            for (auto imported_name: n->import().path) {
                // imported[n->import().path.back()] = path_resolver(n->import().path);
            }
        }
    }

    for (auto n: imported) {
        std::string imported_module_name = n.first;
        if (compiled_modules.count(imported_module_name) == 0) {
            std::string imported_module_path = path_join(u_dirname(__file__), imported_module_name + ".xl");
            if (!file_exists(imported_module_path)) {
                Errors::module_not_found(__file__, imported_module_name);
                exit(1);
            }
            full_compile(false, imported_module_path, output_dir);
        }
    }


    std::string includes = "#include \"runtime/core/core.h\"\n";

    // std::cout << "STARTING!" << std::endl;

    for (auto q: imported) {
        std::string imported_module_name = q.first;
        std::set<std::string> imported_names = q.second;
        for (auto imported_name: imported_names) {
            // std::cout << "HERE setting" << std::endl;
            if (module_maps[imported_module_name]->find(imported_name) == module_maps[imported_module_name]->end()) {
                Errors::name_not_exported_by_module(__file__, imported_module_name, imported_name);
                exit(1);
            } else {
                std::string actual_name = (*module_maps[imported_module_name])[imported_name];
                if (global_classes->declared(actual_name)) {
                    // add all methods as well!
                    ClassInfo* ci = global_classes->get(actual_name);
                    for (auto m: ci->methods) {
                        map[imported_name + "." + m.first] = (*module_maps[imported_module_name])[imported_name + "." +
                                                                                                  m.first];
                    }
                }
                map[imported_name] = actual_name;
            }
        }
        includes += "#include \"" + imported_module_name + ".h\"\n";
    }
    std::map<std::string, std::string> foo;
    try {
        GlobalProcessor gp(global_classes, global_functions, module_name, foo);
        gp.__file__ = __file__;
        gp.visit(*tree);
        // Checker checker(map, global_classes, global_functions);
        // checker.__file__ = __file__;
        // checker.code_lines = code_lines;
        // checker.visit(*tree);
        // if (checker.failed) {
        //     throw std::runtime_error("ERROR");
        // }
    } catch (const std::runtime_error& e) {
        std::cerr << "THERE WAS A SEMANTIC ERROR: " << e.what() << std::endl;
        exit(1);
    }
    std::string source;
    std::string header;
    Transpiler t(map, module_name, includes);
    try {
        t.transpile(tree);
        source = t.m_source;
        header = t.m_header;
        // output += t.globals_initialization;
        // output += code;
    } catch (...) {
        std::cerr << "THERE WAS ATRANPILE ERROR" << std::endl;
        exit(2);
    }
    if (is_main) {
        source += "\nint main(){\n";
        source += t.static_initializations;
        source += "auto x = GET_INT(CALL0(" + map["main"] + "));\n";
        source += t.static_cleanups;
        source += "return x;\n}";
    }
    std::string output_h_path = path_join(output_dir, module_name + ".h");

    // delete tree;

    // header = includes + header;

    std::string output_cpp_path = path_join(output_dir, module_name + ".cpp");
    // std::cout << "Outputting " << output_cpp_path << std::endl;
    std::ofstream output_cpp_file(output_cpp_path);
    // std::cout << "OUTPUT: " << source;
    output_cpp_file << source;

    // std::cout << "Outputting " << output_h_path << std::endl;
    std::ofstream output_h_file(output_h_path);
    // std::cout << "OUTPUT: " << header;
    output_h_file << header;
    std::cout << "DONE COMPILING MODULE : " << module_name << std::endl;
    compiled_modules.insert(module_name);
}

static std::map<std::string, Package*> global_packages;
static std::map<std::string, Module*> global_modules;

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
                    package->children[module_name] = module;
                    module->dotted_path = (package->dotted_path != "" ? package->dotted_path + "." : "") + module_name;
                    global_modules[module->dotted_path] = module;
                } else {
                    std::cout << std::string(level + 1, '\t') << "OTHER: " << d_name << std::endl;
                }
            } else if (ent->d_type == DT_DIR) {
                std::string subpackage_path = package->path + "/" + d_name;
                std::string subpackage_name = d_name;
                Package* subpackage = new Package(d_name, subpackage_path);
                package->children[d_name] = subpackage;
                subpackage->dotted_path =
                        (package->dotted_path != "" ? package->dotted_path + "." : "") + subpackage_name;
                build_packages(subpackage, level + 1);
                global_packages[subpackage->dotted_path] = subpackage;
            }
        }
        ent = readdir(dir);
    }
}

void parse_all_modules(Package* package) {
    std::cout << "Parsing package " << package->name << std::endl;
    for (auto m: package->children) {
        if (m.second->type == UnitType::PACKAGE) {
            parse_all_modules((Package*) m.second);
        } else {
            CodeLines code_lines;
            std::cout << "Parsing module " << m.second->name << std::endl;
            BlockNode* ast = full_parse(m.second->path, &code_lines);
            std::cout << "- Done" << std::endl;
            ((Module*) m.second)->ast = ast;
            ((Module*) m.second)->code_lines = code_lines;
        }
    }
}

static std::map<std::string, std::string> global_path_to_mangled_name;

void process_global_all_modules(Package* package) {
    std::cout << "Global processing package " << package->name << std::endl;
    for (auto m: package->children) {
        if (m.second->type == UnitType::PACKAGE) {
            process_global_all_modules((Package*) m.second);
        } else {
            Module* module = (Module*) m.second;
            std::cout << "Global processing module " << module->name << std::endl;
            GlobalProcessor gp(global_classes, global_functions, module->name, global_path_to_mangled_name);
            gp.module_dotted_path = module->dotted_path;
            gp.__file__ = module->path;
            gp.visit(*module->ast);
            module->imported_paths = gp.imported_paths;
            module->local_paths = gp.local_paths;
            std::cout << "- Done" << std::endl;
        }
    }
}


void analyze_all_modules(Package* package) {
    std::cout << "Analyzing package " << package->name << std::endl;
    for (auto m: package->children) {
        if (m.second->type == UnitType::PACKAGE) {
            analyze_all_modules((Package*) m.second);
        } else {
            Module* module = (Module*) m.second;
            std::cout << "Analyzing module " << module->name << std::endl;
            module->imported_paths["print"] = "core.print";
            module->imported_paths["Integer"] = "core.Integer";
            module->imported_paths["String"] = "core.String";
            Checker checker(module->imported_paths, module->local_paths, global_classes, global_functions);
            checker.global_packages = &global_packages;
            checker.global_modules = &global_modules;
            checker.__file__ = module->path;
            checker.code_lines = module->code_lines;
            checker.visit(*module->ast);
            if (checker.failed) {
                throw std::runtime_error("ERROR");
            }
            std::cout << "- Done" << std::endl;
        }
    }
}

static Package* root_package;


void transpile_all_modules(Package* package) {
    // std::cout << "Transpiling package " << package->name << std::endl;
    // for (auto m: package->children) {
    //     if (m.second->type == UnitType::PACKAGE) {
    //         transpile_all_modules((Package*) m.second);
    //     } else {
    //         Module* module = (Module*) m.second;
    //         Transpiler t;
    //         t.visit_block(*module->ast);
    //         std::cout << "Analyzing module " << module->name << std::endl;
    //         module->imported_paths["print"] = "core.print";
    //         module->imported_paths["Integer"] = "core.Integer";
    //         module->imported_paths["String"] = "core.String";
    //         Checker checker(module->imported_paths, module->local_paths, global_classes, global_functions);
    //         checker.global_packages = &global_packages;
    //         checker.global_modules = &global_modules;
    //         checker.__file__ = module->path;
    //         checker.code_lines = module->code_lines;
    //         checker.visit(*module->ast);
    //         if (checker.failed) {
    //             throw std::runtime_error("ERROR");
    //         }
    //         std::cout << "- Done" << std::endl;
    //     }
    // }
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

    class_builtins["File"]["read_line"] = "fun()->String";
    class_builtins["Integer"]["str"] = "fun(Integer)->String";
    class_builtins["Float"]["str"] = "fun(Float)->String";
    class_builtins["Double"]["str"] = "fun(Double)->String";
    class_builtins["List"]["len"] = "fun(List[a])->Integer";
    class_builtins["List"]["pop"] = "fun(List[a],a)";
    class_builtins["List"]["push"] = "fun(List[a])->a";
    class_builtins["List"]["unordered_map"] = "fun(fun(t)->b)->List[b]";
    class_builtins["String"]["len"] = "fun(String)->Integer";
    class_builtins["Boolean"] = {};


    for (auto fb: function_builtins) {
        global_functions->add("core." + fb.first, parse_function_type(fb.second));
    }

    global_classes->set("core.Float", make_float_class_info());
    global_classes->set("core.Double", make_double_class_info());
    global_classes->set("core.File", make_file_class_info());
    global_classes->set("core.Integer", make_int_class_info());
    global_classes->set("core.List", make_list_class_info());
    global_classes->set("core.Boolean", make_boolean_class_info());
    global_classes->set("core.String", make_string_class_info());

    for (auto cb: class_builtins) {
        // for (auto m: cb.second) {
        // global_functions->add("core." + cb.first, parse_function_type(m.second));
        // }
    }

    analyze_all_modules(root_package);
    transpile_all_modules(root_package);


    // full_compile(true, __main_file__, output_dir);
    //
    // for (auto ct: module_exported_classes) {
    //     delete ct.second;
    // }
    // for (auto ft: module_exported_functions) {
    //     delete ft.second;
    // }
}