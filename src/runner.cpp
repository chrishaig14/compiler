#include <fstream>
#include <sstream>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "semantic/GlobalProcessor.h"
#include "semantic/Checker.h"
#include "translator/Translator.h"
#include "vm/Loader.h"
#include "vm/CodeRunner.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>
#include <spdlog/stopwatch.h>

void compile_and_run(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Token token;
    for (auto token: tokens) {
        std::cout << token.to_string() << std::endl;
    }
    Parser parser(tokens);
    BlockNode* program;
    try {
        auto parse_stopwatch = spdlog::stopwatch();
        program = parser.parse_program();
        spdlog::get("main")->info("{:<32}\tTotal time: {:03.9f} seconds", "Finished parsing", parse_stopwatch);
    } catch (const UnexpectedToken& ut) {
        std::cerr << ut.what() << std::endl;
        exit(1);
    }
    std::vector<Builtin> builtins;
    Translator translator;

    try {
        auto check_stopwatch = spdlog::stopwatch();
        GlobalProcessor gp(builtins);
        gp.visit(*program);
        Checker checker(gp.globals, gp.class_table, gp.function_table);
        checker.visit(*program);
        auto end = std::chrono::steady_clock::now();
        spdlog::get("main")->info("{:<32}\tTotal time: {:03.9f} seconds", "Finished checking", check_stopwatch);
//        program->accept(translator);
        auto translate_stopwatch = spdlog::stopwatch();
        translator.visit(*program);
        spdlog::get("main")->info("{:<32}\tTotal time: {:03.9f} seconds", "Finished translating", translate_stopwatch);
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
    CodeRunner code_runner(main_function->stuff.user->code, structs, stack, global_env);
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
    spdlog::set_level(spdlog::level::debug);
    spdlog::stdout_color_mt("main");
    compile_and_run(text);
}