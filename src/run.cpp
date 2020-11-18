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
    Parser parser(tokens);
    BlockNode* program;
    try {
        program = parser.parse_program();
    } catch (const UnexpectedToken& ut) {
        std::cerr << ut.what() << std::endl;
        exit(1);
    }
    std::vector<Builtin> builtins;
    std::cout << "Parser check passed!" << std::endl;

    GlobalProcessor gp(builtins);
    gp.visit(*program);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.function_table = gp.function_table;
    checker.visit(*program);
    std::cout << "Semantic check passed!" << std::endl;
//    Translator translator;
//    program->accept(translator);
//    ObjectStack stack;
//    StructProtos structs;
//    CodeLabel translated_code = translator.code;
////    std::cerr << translated_code << std::endl;
//    Loader loader(translated_code, builtins);
//    loader.load();
//    Environment* global_env = loader.global_env;
//    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main.0"));
//    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
//    code_runner.run();
}

