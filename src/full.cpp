#include <sstream>
#include <fstream>
#include <fmt/ostream.h>
#include <fmt/color.h>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "semantic/GlobalProcessor.h"
#include "semantic/Checker.h"
#include "transpiler/Transpiler.h"
#include "logging/logging.h"
#include "semantic/util.h"

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

class Mapping {
public:
    bool is_function;
    bool is_class;
    bool found;
    const void* ptr;

    Mapping(const ClassInfo* p) {
        this->ptr = p;
        this->found = true;
        this->is_class = true;
        this->is_function = false;
    }


    Mapping(const FunctionType* p) {
        this->ptr = p;
        this->found = true;
        this->is_function = true;
        this->is_class = false;
    }

    Mapping() {
        this->ptr = nullptr;
        this->found = false;
        this->is_class = false;
        this->is_function = false;
    }

    const ClassInfo* get_class() {
        return (const ClassInfo*) ptr;
    }

    const FunctionType* get_function() {
        return (const FunctionType*) ptr;
    }
};

class ModuleMapping {
    ClassTable* class_table;
    FunctionTable* function_table;

    Mapping find(const std::string& name) {
        if (class_table->declared(name)) {
            return Mapping(class_table->get(name));
        } else if (function_table->has_function(name)) {
            return Mapping(&function_table->get(name));
        }
        return Mapping();
    }
};


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
            for (auto imported_name: n->import().imports) {
                imported[n->import().module_name].insert(imported_name);
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

    try {
        GlobalProcessor gp(module_maps, global_classes, global_functions, module_name);
        gp.__file__ = __file__;
        gp.visit(*tree);
        Checker checker(map, global_classes, global_functions);
        checker.__file__ = __file__;
        checker.code_lines = code_lines;
        checker.visit(*tree);
        if (checker.failed) {
            throw std::runtime_error("ERROR");
        }
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

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << style(RED, "Error: expected 2 args: source_dir output_dir") << std::endl;
        exit(1);
    }
    std::string project_dir = argv[1];
    std::string output_dir = argv[2];
    std::string __main_file__ = path_join(project_dir, u_basename(project_dir) + ".xl");
    std::cout << style(BLUE, "Main file: ") << style(MAGENTA, __main_file__) << std::endl;

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
        std::string mangled_name = mangle_function_name("core", fb.first);
        builtins[fb.first] = mangled_name;
        global_functions->add(mangled_name, parse_function_type(fb.second));
    }

    global_classes->set(mangle_class_name("core", "Float"), make_float_class_info());
    global_classes->set(mangle_class_name("core", "Double"), make_double_class_info());
    global_classes->set(mangle_class_name("core", "File"), make_file_class_info());
    global_classes->set(mangle_class_name("core", "Integer"), make_int_class_info());
    global_classes->set(mangle_class_name("core", "List"), make_list_class_info());
    global_classes->set(mangle_class_name("core", "Boolean"), make_boolean_class_info());
    global_classes->set(mangle_class_name("core", "String"), make_string_class_info());

    for (auto cb: class_builtins) {
        for (auto m: cb.second) {
            std::string mangled_name = mangle_method_name("core", cb.first, m.first);
            builtins[cb.first + "." + m.first] = mangled_name;
            global_functions->add(mangled_name, parse_function_type(m.second));
        }
    }

    full_compile(true, __main_file__, output_dir);

    for (auto ct: module_exported_classes) {
        delete ct.second;
    }
    for (auto ft: module_exported_functions) {
        delete ft.second;
    }
}