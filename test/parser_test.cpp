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

TEST(parser_test, binop_a_eq_b) {
    std::string text = "a==b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_bop(BinopType::EQ, w_id("a"), w_id("b"));
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
    std::string text = "fun foo()->String{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    EXPECT_EQ(equal(node, i_fun("foo", {}, {}, i_type("String", {}), {})), true);
}

TypeNode* n_complex_type_1 = i_type("String", {i_type("List", {t_integer()})});

TEST(parser_test, function_with_params_empty_body) {
    std::string text = "fun foo(x:String[List[Integer]])->Integer{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    EXPECT_EQ(
            equal(node, i_fun("foo", {"x"}, {n_complex_type_1}, i_type("Integer",{}), {})),
            true);
}

TEST(parser_test, function_with_params_and_body) {
    std::string text = "fun foo(x:String[List[Integer]])->String{" + body_1_string + "}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    EXPECT_EQ(
            equal(node, i_fun("foo", {"x"}, {n_complex_type_1}, i_type("String",{}), body_1_node)),
            true);
}

std::string fun_foo_string = "fun foo(x:String[List[Integer]])->List[Integer]{" + body_1_string + "}";
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
            equal(node, i_class("Foo", {}, {}, {})), true);
}

TEST(parser_test, class_foo_with_fields) {
    std::string text = "class Foo{var x: String; var y: Integer;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ClassNode* node = parser.parse_class_definition();
    EXPECT_EQ(
            equal(node, i_class("Foo", {}, {i_decl_type("x", t_string(), NULL),
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
            equal(node, i_class("Foo", {}, {}, {fun_foo_node})),
            true);
}

std::string complete_foo_class_string = "class Foo{var x: String; var y: Integer;" + fun_foo_string + "}";
ClassNode* complete_foo_class_node = i_class("Foo", {}, {i_decl_type("x", t_string(), NULL),
                                                         i_decl_type("y", t_integer(), NULL)},
                                             {fun_foo_node});

TEST(parser_test, template_class_foo_empty) {
    std::string text = "class Foo[T, X]{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ClassNode* node = parser.parse_class_definition();
    EXPECT_EQ(
            equal(node, i_class("Foo", {"T", "X"}, {}, {})),
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

TEST(parser_test, simple_member) {
    std::string text = "a.b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* member_node = w_member(w_id("a"), "b");
    EXPECT_EQ(
            equal(node, member_node),
            true);
}

TEST(parser_test, simple_id) {
    std::string text = "a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_id("a");
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, simple_call) {
    std::string text = "a()";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_call(w_id("a"), {});
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, simple_subscript) {
    std::string text = "a[1]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_sub(w_id("a"), w_num(1));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, literal_number_expression) {
    std::string text = "1";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_num(7);
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, simple_parenthesized_expression) {
    std::string text = "(1)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_num(1);
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, plus_parenthesized_expression) {
    std::string text = "(1+a)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_bop(BinopType::PLUS, w_num(1), w_id("a"));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, more_complex_expression) {
    std::string text = "(1+a)+b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_bop(BinopType::PLUS, w_bop(BinopType::PLUS, w_num(1), w_id("a")), w_id("b"));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, more_complex_expression_2) {
    std::string text = "b*(1+a)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_bop(BinopType::TIMES, w_id("b"), w_bop(BinopType::PLUS, w_num(1), w_id("a")));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, plus_expression) {
    std::string text = "1+a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_add_or_sub_expression();
    AstNode* expected_node = w_bop(BinopType::PLUS, w_num(1), w_id("a"));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, minus_expression) {
    std::string text = "a-1";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_add_or_sub_expression();
    AstNode* expected_node = w_bop(BinopType::MINUS, w_id("a"), w_num(1));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, plus_or_minus_with_multiple_terms_expression) {
    std::string text = "a+b-c";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_add_or_sub_expression();
    AstNode* expected_node = w_bop(BinopType::MINUS, w_bop(BinopType::PLUS, w_id("a"), w_id("b")), w_id("c"));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, mul_or_div_with_multiple_factors_expression) {
    std::string text = "a/b*c";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_add_or_sub_expression();
    AstNode* expected_node = w_bop(BinopType::TIMES, w_bop(BinopType::DIV, w_id("a"), w_id("b")), w_id("c"));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, times_expression) {
    std::string text = "foo*bar";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_mul_or_div_expression();
    AstNode* expected_node = w_bop(BinopType::TIMES, w_id("foo"), w_id("bar"));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, div_expression) {
    std::string text = "foo/1";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_mul_or_div_expression();
    AstNode* expected_node = w_bop(BinopType::DIV, w_id("foo"), w_num(1));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, complex_div_expression) {
    std::string text = "foo/(1)+a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_add_or_sub_expression();
    AstNode* expected_node = w_bop(BinopType::PLUS, w_bop(BinopType::DIV, w_id("foo"), w_num(1)), w_id("a"));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, complex_expression) {
    std::string text = "1-(7)*8-(9*(1-3)/7-8+4)+8";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_bop(BinopType::PLUS, w_bop(BinopType::MINUS, w_bop(BinopType::MINUS, w_num(1),
                                                                                  w_bop(BinopType::TIMES, w_num(7),
                                                                                        w_num(8))),
                                                          w_bop(BinopType::PLUS, w_bop(BinopType::MINUS,
                                                                                       w_bop(BinopType::DIV,
                                                                                             w_bop(BinopType::TIMES,
                                                                                                   w_num(9),
                                                                                                   w_bop(BinopType::MINUS,
                                                                                                         w_num(1),
                                                                                                         w_num(3))),
                                                                                             w_num(7)), w_num(8)),
                                                                w_num(4))), w_num(8));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}


TEST(parser_test, complex_chain) {
    std::string text = "a[1][b].c(1,d[5][0].e).f.g[h][2]()[3][5].i";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node =
            w_member(
                    w_sub(
                            w_sub(
                                    w_call(
                                            w_sub(
                                                    w_sub(
                                                            w_member(
                                                                    w_member(
                                                                            w_call(
                                                                                    w_member(
                                                                                            w_sub(
                                                                                                    w_sub(
                                                                                                            w_id("a"),
                                                                                                            w_num(1)),
                                                                                                    w_id("b")),
                                                                                            "c"),
                                                                                    {w_num(1), w_member(
                                                                                            w_sub(w_sub(
                                                                                                    w_id("d"),
                                                                                                    w_num(5)),
                                                                                                  w_num(0)),
                                                                                            "e")}), "f"), "g"),
                                                            w_id("h")),
                                                    w_num(2)), {}), w_num(3)), w_num(5)),
                    "i");
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}

TEST(parser_test, super_expression) {
    std::string text = "1-(a*c()[0]+7/d.a.x(7))*v*c/a+v.x.y[0][1][a+c*7](4,1,b+c)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    AstNode* node = parser.parse_expression();
    AstNode* expected_node = w_bop(BinopType::PLUS,
                                   w_bop(BinopType::MINUS, w_num(1), w_bop(BinopType::DIV, w_bop(BinopType::TIMES,
                                                                                                 w_bop(BinopType::TIMES,
                                                                                                       w_bop(BinopType::PLUS,
                                                                                                             w_bop(BinopType::TIMES,
                                                                                                                   w_id("a"),
                                                                                                                   w_sub(w_call(
                                                                                                                           w_id("c"),
                                                                                                                           {}),
                                                                                                                         w_num(
                                                                                                                                 0))),
                                                                                                             w_bop(BinopType::DIV,
                                                                                                                   w_num(7),
                                                                                                                   w_call(w_member(
                                                                                                                           w_member(
                                                                                                                                   w_id("d"),
                                                                                                                                   "a"),
                                                                                                                           "x"),
                                                                                                                          {w_num(7)}))),
                                                                                                       w_id("v")),
                                                                                                 w_id("c")),
                                                                           w_id("a"))), w_call(
                    w_sub(w_sub(w_sub(w_member(w_member(w_id("v"), "x"), "y"), w_num(0)), w_num(1)),
                          w_bop(BinopType::PLUS, w_id("a"), w_bop(BinopType::TIMES, w_id("c"), w_num(7)))),
                    {w_num(4), w_num(1), w_bop(BinopType::PLUS, w_id("b"), w_id("c"))}));
    EXPECT_EQ(
            equal(node, expected_node),
            true);
}