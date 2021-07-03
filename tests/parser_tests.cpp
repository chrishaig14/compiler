#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../src/scanner/Scanner.h"
#include "../src/parser/Parser.h"

TEST_CASE("asdf", "[parser]") {
    Scanner scanner;
    std::string code = "fun main()->Integer{}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";
    BlockNode* ast;
    try {
        ast = parser.parse_program();
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        exit(0);
    }
}

TEST_CASE("asdfasdfwerw", "[parser]") {
    Scanner scanner;
    std::string code = "fun main()->Integer{}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";
    FunctionNode* ast;
    try {
        ast = parser.parse_function_definition();
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        exit(0);
    }

    REQUIRE(ast->identifier == "main");
    REQUIRE(ast->parameter_names == VectorOfStrings({}));
    REQUIRE(*ast->return_type == ObjectType("Inteager"));
}