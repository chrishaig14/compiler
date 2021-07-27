#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../src/scanner/Scanner.h"
#include "../src/parser/Parser.h"

const TextPosition DUMMY_POS = {0, 0};

struct TestNode {
    std::string text;
    Node* node;
};

struct TestTypeNode {
    std::string text;
    TypeNode* node;
};

const std::string& ID = "foo";
const std::string& ID_1 = "foo";
const std::string& ID_2 = "bar";

const TestTypeNode TYPE{"Integer", new ObjectType("Integer")};
const TestTypeNode TYPE_1{"String", new ObjectType("String")};
const TestTypeNode TYPE_2{"Integer", new ObjectType("Integer")};

const TestNode EXP_ID_1{"foo", new IdNode("foo", DUMMY_POS, DUMMY_POS)};
const TestNode EXP_ID_2{"bar", new IdNode("bar", DUMMY_POS, DUMMY_POS)};

const TestNode EXPRESSION{"x", new IdNode("x", DUMMY_POS, DUMMY_POS)};
const TestNode FACTOR_EXPRESSION = EXPRESSION;
const TestNode EXPRESSION_1{EXP_ID_1.text + "+" + EXP_ID_2.text,
                            new BinopNode(OpType::ADD, EXP_ID_1.node, EXP_ID_2.node, DUMMY_POS, DUMMY_POS)};
const TestNode EXPRESSION_2{EXP_ID_2.text + "*" + EXP_ID_1.text,
                            new BinopNode(OpType::MUL, EXP_ID_2.node, EXP_ID_1.node, DUMMY_POS, DUMMY_POS)};

const TestNode DECLARATION{"var " + ID + " = " + EXPRESSION.text,
                           new DeclarationNode(ID, nullptr, EXPRESSION.node, DUMMY_POS, DUMMY_POS, DUMMY_POS)};

const TestNode EMPTY_BLOCK{"{}", new BlockNode({}, DUMMY_POS, DUMMY_POS)};

const TestNode IF{"if(" + EXPRESSION.text + ")" + EMPTY_BLOCK.text,
                  new IfNode(EXPRESSION.node, (BlockNode*) EMPTY_BLOCK.node, {}, nullptr, DUMMY_POS, DUMMY_POS)};
const TestNode BLOCK{"{" + DECLARATION.text + ";" + IF.text + "}",
                     new BlockNode({DECLARATION.node, IF.node}, DUMMY_POS, DUMMY_POS)};

const ObjectType NO_TYPE(".None");

TEST_CASE("decl_simple", "[parser]") {
    Scanner scanner;
    std::string code = DECLARATION.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    DeclarationNode* ast = parser.parse_variable_declaration();

    REQUIRE(ast->to_json() == DECLARATION.node->to_json());
}

TEST_CASE("decl_with_type", "[parser]") {
    Scanner scanner;
    std::string code = "var " + ID + " : " + TYPE.text + " = " + EXPRESSION.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    DeclarationNode* ast = parser.parse_variable_declaration();

    REQUIRE(ast->identifier == ID);
    REQUIRE(ast->type->to_json() == TYPE.node->to_json());
    REQUIRE(ast->expression->to_json() == EXPRESSION.node->to_json());
}

TEST_CASE("if", "[parser]") {
    Scanner scanner;
    std::string code = IF.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    IfNode* ast = parser.parse_if();
    REQUIRE(ast->to_json() == IF.node->to_json());
}

TEST_CASE("call_no_args", "[parser]") {
    Scanner scanner;
    std::string code = FACTOR_EXPRESSION.text + "()";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_expression();
    REQUIRE(ast->to_json() == CallNode(FACTOR_EXPRESSION.node, {}, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("call_one_arg", "[parser]") {
    Scanner scanner;
    std::string code = FACTOR_EXPRESSION.text + "(" + EXPRESSION_1.text + ")";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_expression();
    REQUIRE(ast->to_json() == CallNode(FACTOR_EXPRESSION.node, {EXPRESSION_1.node}, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("call_mult_arg", "[parser]") {
    Scanner scanner;
    std::string code = FACTOR_EXPRESSION.text + "(" + EXPRESSION_1.text + "," + EXPRESSION_2.text + ")";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_expression();
    REQUIRE(ast->to_json() ==
            CallNode(FACTOR_EXPRESSION.node, {EXPRESSION_1.node, EXPRESSION_2.node}, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("for", "[parser]") {
    Scanner scanner;
    std::string code = "for " + ID + " @ " + EXPRESSION_1.text + BLOCK.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    ForNode* ast = parser.parse_for_loop();
    REQUIRE(ast->to_json() == ForNode(ID, EXPRESSION_1.node, (BlockNode*) BLOCK.node, DUMMY_POS, DUMMY_POS).to_json());
}


TEST_CASE("while", "[parser]") {
    Scanner scanner;
    std::string code = "while " + EXPRESSION_1.text + BLOCK.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    WhileNode* ast = parser.parse_while_loop();

    REQUIRE(ast->to_json() == WhileNode(EXPRESSION_1.node, (BlockNode*) BLOCK.node, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("fun_simple", "[parser]") {
    Scanner scanner;
    std::string code = "fun " + ID + "()" + BLOCK.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    FunctionNode* ast = parser.parse_function_definition();

    REQUIRE(ast->to_json() ==
            FunctionNode(ID, {}, {}, NO_TYPE.clone(), (BlockNode*) BLOCK.node, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("fun_one_arg", "[parser]") {
    Scanner scanner;
    std::string code = "fun " + ID + "(" + ID_1 + ":" + TYPE.text + ")" + BLOCK.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    FunctionNode* ast = parser.parse_function_definition();

    REQUIRE(ast->to_json() == FunctionNode(ID,
                                           {ID_1},
                                           {TYPE.node->clone()},
                                           NO_TYPE.clone(),
                                           (BlockNode*) BLOCK.node,
                                           DUMMY_POS,
                                           DUMMY_POS).to_json());
}

TEST_CASE("fun_mult_arg", "[parser]") {
    Scanner scanner;
    std::string code = "fun " + ID + "(" + ID_1 + ":" + TYPE_1.text + "," + ID_2 + ":" + TYPE_2.text + ")" + BLOCK.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    FunctionNode* ast = parser.parse_function_definition();

    REQUIRE(ast->to_json() == FunctionNode(ID,
                                           {ID_1, ID_2},
                                           {TYPE_1.node->clone(), TYPE_2.node->clone()},
                                           NO_TYPE.clone(),
                                           (BlockNode*) BLOCK.node,
                                           DUMMY_POS,
                                           DUMMY_POS).to_json());
}