#include <fstream>
#include <sstream>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "semantic/GlobalProcessor.h"
#include "semantic/Checker.h"
#include "translator/Translator.h"
#include "vm/Loader.h"
#include "vm/CodeRunner.h"


void compile_and_run(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Token token;
    for (auto token: tokens) {
        std::cout << token.to_string() << std::endl;
    }
    Parser parser(tokens);
    NodeContainer program;
    try {
        program = parser.parse_program();
    } catch (const UnexpectedToken& ut) {
        std::cerr << ut.what() << std::endl;
        exit(1);
    }
    std::vector<Builtin> builtins;
    Translator translator;

    try {
        GlobalProcessor gp(builtins);
        gp.visit(*program.node.block);
        Checker checker(gp.globals, gp.class_table);
        checker.function_table = gp.function_table;
        checker.dispatch(program);
//        program->accept(translator);
        translator.dispatch(program);
    } catch (const std::runtime_error& e) {
        std::cerr << "THERE WAS A SEMANTIC ERROR: " << e.what() << std::endl;
        exit(1);
    }
    ObjectStack stack;
    StructProtos structs;
    CodeLabel translated_code = translator.code;
    std::cerr << translated_code << std::endl;
    Loader loader(translated_code, builtins);
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    try {
        code_runner.run();
        auto x = stack;
    } catch (const std::runtime_error& e) {
        std::cerr << "THERE WAS A RUNTIME ERROR: " << e.what() << std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Error: expected a file name to compile" << std::endl;
        exit(1);
    }
    std::string filename = argv[1];
    std::ifstream file(filename);
    std::stringstream sstream;
    sstream << file.rdbuf();
    std::string text = sstream.str();
    compile_and_run(text);
}