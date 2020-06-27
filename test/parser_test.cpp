//
// Created by chris on 14/6/20.
//

#include <gtest/gtest.h>
#include <Scanner.h>
#include <Parser.h>
#include <gtest/gtest.h>
#include <Scanner.h>
#include <Parser.h>
#include <AstNode.h>
#include "utils.h"

typedef std::vector<TypeNode*> VectorOfTypes;
typedef std::vector<std::string> VectorOfStrings;
typedef std::vector<AstNode*> VectorOfNodes;

AstNode* w_id(std::string name) {
    AstNode* ast_node = new AstNode;
    ast_node->type = AstType::IDENTIFIER;
    ast_node->ast_identifier = new IdentifierNode(name);
    return ast_node;
}

AstNode* w_asn(AstNode* lvalue, AstNode* rvalue) {
    AstNode* ast_node = new AstNode;
    ast_node->type = AstType::ASSIGNMENT;
    ast_node->ast_assignment = new AssignmentNode(lvalue, rvalue);
    return ast_node;
}

AstNode* w_decl(std::string name, AstNode* expression) {
    AstNode* ast_node = new AstNode;
    ast_node->type = AstType::DECLARATION;
    ast_node->ast_declaration = new DeclarationNode(name, NULL, NULL);
    return ast_node;
}

AstNode* w_class(ClassNode* node) {
    AstNode* ast_node = new AstNode;
    ast_node->type = AstType::CLASS;
    ast_node->ast_class = node;
    return ast_node;
}

AstNode* w_fun(FunctionNode* node) {
    AstNode* ast_node = new AstNode;
    ast_node->type = AstType::FUNCTION;
    ast_node->ast_function = node;
    return ast_node;
}

AstNode* w_bop(BinopType op, AstNode* left, AstNode* right) {
    AstNode* ast_node = new AstNode;
    ast_node->type = AstType::BINOP;
    ast_node->ast_binop = new BinopNode(op, left, right);
    return ast_node;
}


AstNode* n_a_plus_b = w_bop(BinopType::PLUS, w_id("a"), w_id("b"));

DeclarationNode* n_decl_x = new DeclarationNode("x", NULL, NULL);


AstNode* n_asn_x(AstNode* rvalue) {
    return w_asn(w_id("x"), rvalue);
}

TEST(parser_test, binop_a_plus_b) {
    std::string text = "a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_add_or_sub_expression();
    AstNode* expected_node = n_a_plus_b;
    EXPECT_EQ(equal(node, expected_node), true);
}

TEST(parser_test, exp_identifier) {
    std::string text = "foo";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_add_or_sub_expression();
    EXPECT_EQ(equal(node, w_id("foo")), true);
}

TEST(parser_test, assign_x_equal_y) {
    std::string text = "x = y";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_assignment_or_expression();
    EXPECT_EQ(equal(node, n_asn_x(w_id("y"))), true);
}

TEST(parser_test, assign_x_equal_binop_a_plus_b) {
    std::string text = "x = a+b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_assignment_or_expression();
    EXPECT_EQ(equal(node, n_asn_x(n_a_plus_b)), true);
}

TEST(parser_test, decl_x_without_value) {
    std::string text = "var x";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    DeclarationNode* node = parser.parse_variable_declaration();
    EXPECT_EQ(equal(node, n_decl_x), true);
}

TEST(parser_test, decl_x_with_value) {
    std::string text = "var x = a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    DeclarationNode* node = parser.parse_variable_declaration();
    EXPECT_EQ(equal(node, i_decl("x", n_a_plus_b)), true);
}

TEST(parser_test, empty_block) {
    std::string text = "{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    VectorOfNodes node = parser.parse_possibly_empty_block();
    EXPECT_EQ(node.size(), 0);
}


TEST(parser_test, non_empty_block) {
    std::string text = "{x = a + b; x = y;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    VectorOfNodes node = parser.parse_possibly_empty_block();
    EXPECT_EQ(node.size(), 2);
    EXPECT_EQ(equal(node[0], w_asn(w_id("x"), n_a_plus_b)), true);
    EXPECT_EQ(equal(node[1], w_asn(w_id("x"), w_id("y"))), true);
}

TEST(parser_test, if_empty_then) {
    std::string text = "if(x){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    IfNode* node = parser.parse_if();
    EXPECT_EQ(equal(node, i_if(w_id("x"), VectorOfNodes())), true);
}

std::string body_1_string = "x = a + b; x = y;";
VectorOfNodes body_1_node = {w_asn(w_id("x"), n_a_plus_b), w_asn(w_id("x"), w_id("y"))};
TEST(parser_test, if_non_empty_then) {
    std::string text = "if(x){" + body_1_string + "}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    IfNode* node = parser.parse_if();
    EXPECT_EQ(equal(node, i_if(w_id("x"), body_1_node)), true);
}

TEST(parser_test, simple_type) {
    std::string text = "String";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    EXPECT_EQ(equal(node, i_type("String", VectorOfTypes())), true);
}

TEST(parser_test, template_type) {
    std::string text = "String[Integer]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    EXPECT_EQ(equal(node, i_type("String", {t_integer()})), true);
}

TypeNode* n_complex_type = i_type("String", {t_integer(), i_type("List", {t_string()})});

TEST(parser_test, complex_template_type) {
    std::string text = "String[Integer, List[String]]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    EXPECT_EQ(equal(node, n_complex_type), true);
}

TEST(parser_test, decl_with_type) {
    std::string text = "var x:String[Integer, List[String]]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    DeclarationNode* node = parser.parse_variable_declaration();
    EXPECT_EQ(equal(node, i_decl_type("x", n_complex_type, NULL)), true);
}

TEST(parser_test, decl_with_type_and_value) {
    std::string text = "var x:String[Integer, List[String]] = a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    DeclarationNode* node = parser.parse_variable_declaration();
    EXPECT_EQ(equal(node, i_decl_type("x", n_complex_type, n_a_plus_b)), true);
}

TEST(parser_test, function_no_params_empty_body) {
    std::string text = "fun foo(){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    EXPECT_EQ(equal(node, i_fun("foo", {}, {}, NULL, {})), true);
}

TypeNode* n_complex_type_1 = i_type("String", {i_type("List", {t_integer()})});

TEST(parser_test, function_with_params_empty_body) {
    std::string text = "fun foo(x:String[List[Integer]]){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    EXPECT_EQ(
            equal(node, i_fun("foo", {"x"}, {n_complex_type_1}, NULL, {})),
            true);
}

TEST(parser_test, function_with_params_and_body) {
    std::string text = "fun foo(x:String[List[Integer]]){" + body_1_string + "}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    EXPECT_EQ(
            equal(node, i_fun("foo", {"x"}, {n_complex_type_1}, NULL, body_1_node)),
            true);
}

std::string fun_foo_string = "fun foo(x:String[List[Integer]]):List[Integer]{" + body_1_string + "}";
FunctionNode* fun_foo_node = i_fun("foo", {"x"}, {n_complex_type_1}, i_type("List", {t_integer()}),
                                   body_1_node);
TEST(parser_test, function_with_params_return_type_and_body) {
    std::string text = fun_foo_string;
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    EXPECT_EQ(
            equal(node, fun_foo_node),
            true);
}

TEST(parser_test, class_foo_empty) {
    std::string text = "class Foo{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ClassNode* node = parser.parse_class_definition();
    EXPECT_EQ(
            equal(node, i_class("Foo", {}, {}, {}, {})),
            true);
}

TEST(parser_test, class_foo_with_fields) {
    std::string text = "class Foo{var x: String; var y: Integer;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ClassNode* node = parser.parse_class_definition();
    EXPECT_EQ(
            equal(node, i_class("Foo", {}, {}, {i_decl_type("x", t_string(), NULL),
                                                i_decl_type("y", t_integer(), NULL)}, {})),
            true);
}

TEST(parser_test, class_foo_with_method) {
    std::string text = "class Foo{" + fun_foo_string + "}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ClassNode* node = parser.parse_class_definition();
    EXPECT_EQ(
            equal(node, i_class("Foo", {}, {}, {}, {fun_foo_node})),
            true);
}

std::string complete_foo_class_string = "class Foo{var x: String; var y: Integer;" + fun_foo_string + "}";
ClassNode* complete_foo_class_node = i_class("Foo", {}, {}, {i_decl_type("x", t_string(), NULL),
                                                             i_decl_type("y", t_integer(), NULL)},
                                             {fun_foo_node});

TEST(parser_test, template_class_foo_empty) {
    std::string text = "class Foo[T, X]{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ClassNode* node = parser.parse_class_definition();
    EXPECT_EQ(
            equal(node, i_class("Foo", {"T", "X"}, {}, {}, {})),
            true);
}

TEST(parser_test, template_class_with_inherited_foo_empty) {
    std::string text = "class Foo[T, X]:Bar[T], Fizz[X]{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ClassNode* node = parser.parse_class_definition();
    EXPECT_EQ(
            equal(node,
                  i_class("Foo", {"T", "X"}, {i_type("Bar", {i_type("T", {})}), i_type("Fizz", {i_type("X", {})})}, {},
                          {})),
            true);
}


TEST(parser_test, class_foo_with_fields_and_method) {
    std::string text = complete_foo_class_string;
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ClassNode* node = parser.parse_class_definition();
    EXPECT_EQ(
            equal(node, complete_foo_class_node),
            true);
}