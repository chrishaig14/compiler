
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

namespace Errors {
    void module_not_found(std::string __file__, std::string imported_module_name) {
        std::cout << style(RED, "Error in file ") << style(MAGENTA BOLD, __file__) << style(RED, ": ")
                  << style(RED, "imported module ") << style(MAGENTA BOLD, "'" + imported_module_name + "'")
                  << style(RED, " not found")
                  << std::endl;
    }

    void
    name_not_exported_by_module(std::string __file__, std::string imported_module_name, std::string imported_name) {
        std::cout << style(RED, "Error in file ") << style(MAGENTA BOLD, __file__) << style(RED, ": ")
                  << style(RED, "imported module ") << style(MAGENTA BOLD, "'" + imported_module_name + "'")
                  << style(RED, " doesn't export name ") << style(MAGENTA BOLD, "'" + imported_name + "'")
                  << std::endl;
    }
}

BlockNode* full_parse(const std::string& __file__, CodeLines* code_lines) {
    BlockNode* tree;
    Scanner scanner;
    scanner.load_file(__file__);
    std::vector<Token> tokens = scanner.scan_all();
    for (auto token: tokens) {
        std::cout << token.to_string() << std::endl;
    }
    Parser parser(__file__, scanner.code_lines, tokens);
    *code_lines = scanner.code_lines;
    try {
        tree = parser.parse_program();
    } catch(const std::runtime_error& e){
        std::cout << e.what() << std::endl;
        exit(1);
    }
    return tree;
}

static std::map<std::string, FunctionTable*> module_exported_functions;
static std::map<std::string, ClassTable*> module_exported_classes;
static std::set<std::string> compiled_modules;

void full_compile(bool is_main, const std::string& __file__, const std::string& output_dir) {
    std::string module_name = module_from_path(__file__);
    CodeLines code_lines;
    BlockNode* tree = full_parse(__file__, &code_lines);
    std::vector<Builtin> builtins;
    Transpiler t;

    std::map<std::string, std::set<std::string>> imported;

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

    ClassTable* imported_classes = new ClassTable();
    FunctionTable* imported_functions = new FunctionTable();

    std::string includes = "#include \"runtime/core/core.h\"\n";

    for (auto q: imported) {
        std::string imported_module_name = q.first;
        std::set<std::string> imported_names = q.second;
        for (auto imported_name: imported_names) {
            if (!module_exported_functions[imported_module_name]->has_function(imported_name) &&
                !module_exported_classes[imported_module_name]->declared(imported_name)) {
                Errors::name_not_exported_by_module(__file__, imported_module_name, imported_name);
                exit(1);
            } else if (module_exported_classes[imported_module_name]->declared(imported_name)) {
                imported_classes->set(imported_name, module_exported_classes[imported_module_name]->get(imported_name));
            } else if (module_exported_functions[imported_module_name]->has_function(imported_name)) {
                imported_functions->add(
                        imported_name,
                        module_exported_functions[imported_module_name]->get(imported_name));
            }
        }
        includes += "#include \"" + imported_module_name + ".h\"\n";
    }

    try {
        GlobalProcessor gp(builtins, imported_classes, imported_functions);
        gp.__file__ = __file__;
        gp.visit(*tree);
        Checker checker(gp.globals, gp.class_table, gp.function_table);
        checker.__file__ = __file__;
        checker.code_lines = code_lines;
        checker.visit(*tree);
        if (checker.failed) {
            throw std::runtime_error("ERROR");
        }
        module_exported_functions[module_name] = gp.function_table;
        module_exported_classes[module_name] = gp.class_table;

    } catch (const std::runtime_error& e) {
        std::cerr << "THERE WAS A SEMANTIC ERROR: " << e.what() << std::endl;
        exit(1);
    }
    std::string output;
    try {
        std::string code = t.transpile(tree);
        output += t.globals_initialization;
        output += code;
    } catch (...) {
        std::cerr << "THERE WAS ATRANPILE ERROR" << std::endl;
        exit(2);
    }
    if (is_main) {
        output += "\nint main(){\n";
        output += t.static_initializations;
        output += "auto x = PTR_TO_INT((*function_main)());";
        output += "return x;}";
    }
    std::string output_h_path = path_join(output_dir, module_name + ".h");
    std::string full_output_h = includes + t.externs_declaration + t.header;

    includes += "#include \"" + module_name + ".h" + "\"\n";


    std::string full_output = includes + output;

    std::string output_cpp_path = path_join(output_dir, module_name + ".cpp");
    std::cout << "Outputting " << output_cpp_path << std::endl;
    std::ofstream output_cpp_file(output_cpp_path);
    std::cout << "OUTPUT: " << full_output;
    output_cpp_file << full_output;

    std::cout << "Outputting " << output_h_path << std::endl;
    std::ofstream output_h_file(output_h_path);
    std::cout << "OUTPUT: " << full_output_h;
    output_h_file << full_output_h;

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
    full_compile(true, __main_file__, output_dir);
}