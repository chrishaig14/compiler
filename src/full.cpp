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
                    package->modules[module_name] = module;
                } else {
                    std::cout << std::string(level + 1, '\t') << "OTHER: " << d_name << std::endl;
                }
            } else if (ent->d_type == DT_DIR) {
                std::string subpackage_path = package->path + "/" + d_name;
                std::string subpackage_name = d_name;
                Package* subpackage = new Package(d_name, subpackage_path);
                package->subpackages[d_name] = subpackage;
                build_packages(subpackage, level + 1);
                package->subpackages[subpackage_name] = subpackage;
            }
        }
        ent = readdir(dir);
    }
}

void parse_all_modules(Package* package) {
    std::cout << "Parsing package " << package->name << std::endl;
    for (auto m: package->subpackages) {
        parse_all_modules((Package*) m.second);
    }
    for (auto m: package->modules) {
        CodeLines code_lines;
        std::cout << "Parsing module " << m.second->name << std::endl;
        BlockNode* ast = full_parse(m.second->path, &code_lines);
        std::cout << "- Done" << std::endl;
        ((Module*) m.second)->ast = ast;
        ((Module*) m.second)->code_lines = code_lines;
    }
}

void process_global_all_modules(Package* package) {
    std::cout << "Global processing package " << package->name << std::endl;
    for (auto m: package->subpackages) {
        process_global_all_modules((Package*) m.second);
    }
    for (auto m: package->modules) {
        Module* module = (Module*) m.second;
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

void analyze_all_modules(Package* package) {
    std::cout << "Analyzing package " << package->name << std::endl;
    for (auto m: package->subpackages) {
        analyze_all_modules((Package*) m.second);
    }
    for (auto m: package->modules) {
        Module* module = (Module*) m.second;
        if (module->name == "core") {
            continue;
        }

        // for(auto i: module->imported_paths){
        //     module->imports[i.first] = root_package->modules[i.second[0]]
        // }

        std::cout << "Analyzing module " << module->name << std::endl;
        module->imports["print"] = root_package->modules["core"]->functions["print"];
        module->imports["Integer"] = root_package->modules["core"]->classes["Integer"];
        module->imports["String"] = root_package->modules["core"]->classes["String"];
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


void transpile_all_modules(Package* package, std::string output_dir) {
    std::cout << "Transpiling package " << package->name << std::endl;
    for (auto m: package->subpackages) {
        transpile_all_modules((Package*) m.second, output_dir);
    }
    for (auto m:package->modules) {
        Module* module = (Module*) m.second;
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

        if (module->functions.count("main") != 0) {
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
    class_builtins["Integer"]["add"] = "fun(Integer, Integer)->Integer";
    class_builtins["Float"]["str"] = "fun(Float)->String";
    class_builtins["Double"]["str"] = "fun(Double)->String";
    class_builtins["List"]["len"] = "fun(List[a])->Integer";
    class_builtins["List"]["pop"] = "fun(List[a],a)";
    class_builtins["List"]["push"] = "fun(List[a])->a";
    class_builtins["List"]["unordered_map"] = "fun(fun(t)->b)->List[b]";
    class_builtins["String"]["len"] = "fun(String)->Integer";
    class_builtins["Boolean"] = {};


    Module* core_module = new Module("core", "");
    root_package->modules["core"] = core_module;

    for (auto fb: function_builtins) {
        ConstFunction* cf = new ConstFunction();
        cf->ft = parse_function_type(fb.second);
        core_module->functions[fb.first] = cf;
    }
    core_module->classes["Float"] = make_float_class_info();
    core_module->classes["Double"] = make_double_class_info();
    core_module->classes["File"] = make_file_class_info();
    core_module->classes["Integer"] = make_int_class_info();
    core_module->classes["List"] = make_list_class_info();
    core_module->classes["Boolean"] = make_boolean_class_info();
    core_module->classes["String"] = make_string_class_info();

    for (auto cb: class_builtins) {
    }

    analyze_all_modules(root_package);
    transpile_all_modules(root_package, output_dir + "/application");
}