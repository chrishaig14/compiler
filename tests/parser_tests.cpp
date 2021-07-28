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

const std::string& ID = "baz";
const std::string& ID_1 = "foo";
const std::string& ID_2 = "bar";

const TestTypeNode TYPE{"Integer", new ObjectType("Integer")};
const TestTypeNode TYPE_1{"String", new ObjectType("String")};
const TestTypeNode TYPE_2{"Integer", new ObjectType("Integer")};
const TestTypeNode TYPE_3{"Boolean", new ObjectType("Boolean")};

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
const TestNode ASSIGNMENT{EXP_ID_1.text + " = " + EXPRESSION_1.text,
                          new AssignmentNode(EXP_ID_1.node, EXPRESSION_1.node, DUMMY_POS, DUMMY_POS)};

const TestNode EMPTY_BLOCK{"{}", new BlockNode({}, DUMMY_POS, DUMMY_POS)};

const TestNode IF{"if(" + EXPRESSION.text + ")" + EMPTY_BLOCK.text,
                  new IfNode(EXPRESSION.node, (BlockNode*) EMPTY_BLOCK.node, {}, nullptr, DUMMY_POS, DUMMY_POS)};
const TestNode BLOCK{"{" + DECLARATION.text + ";" + IF.text + "}",
                     new BlockNode({DECLARATION.node, IF.node}, DUMMY_POS, DUMMY_POS)};
const TestNode BLOCK_1{"{" + ASSIGNMENT.text + ";}", new BlockNode({ASSIGNMENT.node}, DUMMY_POS, DUMMY_POS)};

const TestNode FUNCTION{
        "fun " + ID + "(" + ID_1 + ":" + TYPE_1.text + "," + ID_2 + ":" + TYPE_2.text + ")->" + TYPE_3.text +
        BLOCK.text, new FunctionNode(ID,
                                     {ID_1, ID_2},
                                     {TYPE_1.node->clone(), TYPE_2.node->clone()},
                                     TYPE_3.node->clone(),
                                     (BlockNode*) BLOCK.node,
                                     DUMMY_POS,
                                     DUMMY_POS)};

const ObjectType NO_TYPE(".None");

TEST_CASE("assignment", "[parser]") {
    Scanner scanner;
    std::string code = ASSIGNMENT.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_assignment_or_expression();

    REQUIRE(ast->to_json() == ASSIGNMENT.node->to_json());
}

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

TEST_CASE("if_with_else", "[parser]") {
    Scanner scanner;
    std::string code = "if " + EXPRESSION.text + BLOCK.text + "else " + BLOCK_1.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    IfNode* ast = parser.parse_if();
    REQUIRE(ast->to_json() == IfNode(EXPRESSION.node,
                                     (BlockNode*) BLOCK.node,
                                     {},
                                     (BlockNode*) BLOCK_1.node,
                                     DUMMY_POS,
                                     DUMMY_POS).to_json());
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

TEST_CASE("class_empty", "[parser]") {
    Scanner scanner;
    std::string code = "class " + ID + "{}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    ClassNode* ast = parser.parse_class_definition();

    REQUIRE(ast->to_json() == ClassNode(ID, {}, {}, {}, {}, {}, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("class_one_member", "[parser]") {
    Scanner scanner;
    std::string code = "class " + ID + "{" + ID_1 + ":" + TYPE_1.text + ";}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    ClassNode* ast = parser.parse_class_definition();

    REQUIRE(ast->to_json() ==
            ClassNode(ID, {}, {{ID_1, TYPE_1.node->clone()}}, {}, {}, {}, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("class_mult_member", "[parser]") {
    Scanner scanner;
    std::string code = "class " + ID + "{" + ID_2 + ":" + TYPE_2.text + ";" + ID_1 + ":" + TYPE_1.text + ";}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    ClassNode* ast = parser.parse_class_definition();

    REQUIRE(ast->to_json() == ClassNode(ID,
                                        {},
                                        {{ID_2, TYPE_2.node->clone()},
                                         {ID_1, TYPE_1.node->clone()}},
                                        {},
                                        {},
                                        {},
                                        DUMMY_POS,
                                        DUMMY_POS).to_json());
}

TEST_CASE("class_with_method", "[parser]") {
    Scanner scanner;
    std::string code =
            "class " + ID + "{" + ID_2 + ":" + TYPE_2.text + ";" + ID_1 + ":" + TYPE_1.text + ";" + FUNCTION.text + "}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    ClassNode* ast = parser.parse_class_definition();

    REQUIRE(ast->to_json() == ClassNode(ID,
                                        {},
                                        {{ID_2, TYPE_2.node->clone()},
                                         {ID_1, TYPE_1.node->clone()}},
                                        {{((FunctionNode*) FUNCTION.node)->identifier, Method{nullptr,
                                                                                              (FunctionNode*) FUNCTION.node}}},
                                        {},
                                        {},
                                        DUMMY_POS,
                                        DUMMY_POS).to_json());
}

TEST_CASE("class_with_static_method", "[parser]") {
    Scanner scanner;
    std::string code = "class " + ID + "{ static " + FUNCTION.text + "}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    ClassNode* ast = parser.parse_class_definition();

    REQUIRE(ast->to_json() == ClassNode(ID,
                                        {},
                                        {},
                                        {},
                                        {},
                                        {{((FunctionNode*) FUNCTION.node)->identifier, (FunctionNode*) FUNCTION.node}},
                                        DUMMY_POS,
                                        DUMMY_POS).to_json());
}

TEST_CASE("return_nothing", "[parser]") {
    Scanner scanner;
    std::string code = "return;";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    ReturnNode* ast = parser.parse_return();

    REQUIRE(ast->to_json() == ReturnNode(nullptr, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("return_expression", "[parser]") {
    Scanner scanner;
    std::string code = "return " + EXPRESSION.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    ReturnNode* ast = parser.parse_return();

    REQUIRE(ast->to_json() == ReturnNode(EXPRESSION.node, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("list_empty", "[parser]") {
    Scanner scanner;
    std::string code = "[]::" + TYPE.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_list_literal();

    REQUIRE(ast->to_json() == EmptyListNode(TYPE.node->clone(), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("list_one_element", "[parser]") {
    Scanner scanner;
    std::string code = "[" + EXPRESSION_1.text + "]";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_list_literal();

    REQUIRE(ast->to_json() == ListNode({EXPRESSION_1.node}, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("list_mult_elements", "[parser]") {
    Scanner scanner;
    std::string code = "[" + EXPRESSION_1.text + "," + EXPRESSION_2.text + "]";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_list_literal();

    REQUIRE(ast->to_json() == ListNode({EXPRESSION_1.node, EXPRESSION_2.node}, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("number_integer", "[parser]") {
    Scanner scanner;
    std::string code = "89";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == NumberNode(NumberType::INTEGER, "89", DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("number_float", "[parser]") {
    Scanner scanner;
    std::string code = "3.14";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == NumberNode(NumberType::FLOAT, "3.14", DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("empty_string", "[parser]") {
    Scanner scanner;
    std::string code = "\"\"";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == StringNode("", DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("string", "[parser]") {
    Scanner scanner;
    std::string code = "\"hello, world\"";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == StringNode("hello, world", DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("true", "[parser]") {
    Scanner scanner;
    std::string code = "true";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == BooleanNode(true, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("false", "[parser]") {
    Scanner scanner;
    std::string code = "false";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";

    Node* ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == BooleanNode(false, DUMMY_POS, DUMMY_POS).to_json());
}