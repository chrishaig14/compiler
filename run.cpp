#include <fstream>
#include <sstream>
#include "src/scanner/Scanner.h"
#include "src/parser/Parser.h"
#include "src/semantic/GlobalProcessor.h"
#include "src/semantic/Checker.h"
#include "src/translator/Translator.h"
#include "src/vm/Loader.h"
#include "src/vm/CodeRunner.h"

void compile_and_run(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* program;
    try {
        program = parser.parse_program();
    } catch (const UnexpectedToken& ut) {
        std::cerr << ut.what() << std::endl;
        exit(1);
    }
    std::vector<Builtin> builtins;

    GlobalProcessor gp(builtins);
    gp.visit(*program);
    Checker checker(gp.globals, gp.class_table);
    checker.function_table = gp.function_table;
    checker.visit(*program);
    Translator translator;
    program->accept(translator);
    ObjectStack stack;
    StructProtos structs;
    CodeLabel translated_code = translator.code;
    std::cerr << translated_code << std::endl;
    Loader loader(translated_code, builtins);
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main.0"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Expected a file name to run" << std::endl;
        exit(1);
    }
    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        exit(1);
    }
    std::stringstream sstream;
    sstream << file.rdbuf();
    std::string text = sstream.str();
    compile_and_run(text);
}