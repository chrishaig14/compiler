//
// Created by chris on 14/6/20.
//

#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <parser/UnexpectedToken.h>


#define BODY_NODE new BlockNode({ASN(ID("x"), BIN(OpType::ADD, ID("a"), ID("b"))),ASN(ID("x"), ID("y"))})
#define FUN_FOO_NODE FUN("foo", { "x" }, std::vector<TypeNode*>({ COMPLEX_TYPE }), TYPE("List", {T_INT}), BODY_NODE)
#define FUN_FOO_STRING "fun foo(x:List[List[Integer]])->List[Integer]{x=a+b; x = y;}"
#define COMPLEX_TYPE T_LIST(T_LIST(T_INT))

#define COMPLETE_TEST EXPECT_TRUE(node->equal(expected_node)); delete node; delete expected_node;

TEST(parser_test, binop_a_plus_b) {
    std::string text = "a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = new BinopNode(OpType::ADD, ID("a"), ID("b"));
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 4);
    COMPLETE_TEST;
}

TEST(parser_test, ternary) {
    std::string text = "a?7:6";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_ternary();
    Node* expected_node = TERNARY(ID("a"), NUM(7), NUM(6));
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 4);
    COMPLETE_TEST;
}

TEST(parser_test, or_test) {
    std::string text = "a||b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_or_expression();
    Node* expected_node = BIN(OpType::OR, ID("a"), ID("b"));
    COMPLETE_TEST;
}

TEST(parser_test, expression_with_ternary) {
    std::string text = "a||s-c==7?8-9?7:4:10";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = TERNARY(BIN(OpType::OR, ID("a"), BIN(OpType::EQ, BIN(OpType::SUB, ID("s"), ID("c")), NUM(7))),
                                  TERNARY(BIN(OpType::SUB, NUM(8), NUM(9)), NUM(7), NUM(4)), NUM(10));
    COMPLETE_TEST;
}

TEST(parser_test, ternary_fail_1) {
    std::string text = "a?8:6";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_ternary();
    Node* expected_node = TERNARY(ID("a"), NUM(7), NUM(6));
    COMPLETE_TEST;
}

TEST(parser_test, ternary_fail_2) {
    std::string text = "b?7:6";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_ternary();
    Node* expected_node = TERNARY(ID("a"), NUM(7), NUM(6));
    COMPLETE_TEST;
}

TEST(parser_test, ternary_fail_3) {
    std::string text = "a?8:7";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_ternary();
    Node* expected_node = TERNARY(ID("a"), NUM(7), NUM(6));
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 4);
    COMPLETE_TEST;
}

TEST(parser_test, binop_a_eq_b) {
    std::string text = "a==b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = BIN(OpType::EQ, ID("a"), ID("b"));
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 3);
    COMPLETE_TEST;

}

TEST(parser_test, exp_identifier) {
    std::string text = "foo";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    auto expected_node = ID("foo");
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 2);
    COMPLETE_TEST;
}

TEST(parser_test, assign_x_equal_y) {
    std::string text = "x = y";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_assignment_or_expression();
    Node* expected_node = ASN(ID("x"), ID("y"));
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 4);
    COMPLETE_TEST;


}

TEST(parser_test, assign_x_equal_binop_a_plus_b) {
    std::string text = "x = a+b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_assignment_or_expression();
    Node* expected_node = ASN(ID("x"), BIN(OpType::ADD, ID("a"), ID("b")));
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 6);
    COMPLETE_TEST;

}

TEST(parser_test, decl_x_without_value_or_type_throws_error) {
    std::string text = "var x";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    try {
        DeclarationNode* node = parser.parse_variable_declaration();
        FAIL() << "Didn't throw  unexpected token error";
    } catch (...) {

    }
}

TEST(parser_test, decl_x_with_value) {
    std::string text = "var x = a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    DeclarationNode* node = parser.parse_variable_declaration();
    DeclarationNode* expected_node = DECL("x", nullptr, BIN(OpType::ADD, ID("a"), ID("b")));
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 12);
    COMPLETE_TEST;
}

TEST(parser_test, empty_block) {
    std::string text = "{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* node = parser.parse_possibly_empty_block();
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 1);
    EXPECT_EQ(node->nodes.size(), 0);
}

TEST(parser_test, non_empty_block) {
    std::string text = "{x = a + b; x = y;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* node = parser.parse_possibly_empty_block();
    EXPECT_EQ(node->nodes.size(), 2);
    auto expected_node_0 = ASN(ID("x"), BIN(OpType::ADD, ID("a"), ID("b")));
    EXPECT_TRUE(node->nodes[0]->equal(expected_node_0));
    auto expected_node_1 = ASN(ID("x"), ID("y"));
    EXPECT_TRUE(node->nodes[1]->equal(expected_node_1));
    delete expected_node_0;
    delete expected_node_1;
    delete node;
}

TEST(parser_test, if_empty_then) {
    std::string text = "if(x){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    IfNode* node = parser.parse_if();
    auto expected_node = IF(ID("x"), new BlockNode({}));
    COMPLETE_TEST;
}

TEST(parser_test, if_non_empty_then) {
    std::string text = "if(x){x=a+b; x = y;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    IfNode* node = parser.parse_if();
    auto st_1 = ASN(ID("x"), BIN(OpType::ADD, ID("a"), ID("b")));
    auto st_2 = ASN(ID("x"), ID("y"));
    auto expected_node = IF(ID("x"), new BlockNode({st_1, st_2}));
    COMPLETE_TEST;

}

TEST(parser_test, simple_type) {
    std::string text = "String";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    auto expected_node = T_STRING;
    COMPLETE_TEST;
}

TEST(parser_test, template_type) {
    std::string text = "String[Integer]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    auto expected_node = TYPE("String", std::vector<TypeNode*>{OBJECT_TYPE("Integer", {})});
    COMPLETE_TEST;
}

TEST(parser_test, fun_empty) {
    std::string text = "fun() ->String";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    auto expected_node = FUNCTION_TYPE({}, T_STRING);
    COMPLETE_TEST;
}

TEST(parser_test, fun_full) {
    std::string text = "fun(List[String],fun(String)->Integer) -> List[Integer]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    auto expected_node = new FunctionTypeNode({T_LIST(T_STRING), FUNCTION_TYPE({ T_STRING }, T_INT)}, T_LIST(T_INT));
    COMPLETE_TEST;
}


TEST(parser_test, complex_template_type) {
    std::string text = "String[Integer, List[String]]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    auto expected_node = TYPE("String", std::vector<TypeNode*>({T_INT, T_LIST({T_STRING})}));
    COMPLETE_TEST;

}

TEST(parser_test, decl_with_type_and_value) {
    std::string text = "var x:String[Integer, List[String]] = a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    DeclarationNode* node = parser.parse_variable_declaration();
    auto expected_node = DECL("x", TYPE("String", std::vector<TypeNode*>({T_INT, T_LIST({T_STRING})})),
                              BIN(OpType::ADD, ID("a"), ID("b")));
    COMPLETE_TEST;


}

TEST(parser_test, function_no_params_empty_body) {
    std::string text = "fun foo()->String{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    auto return_type = TYPE("String", {});
    auto expected_node = FUN("foo", std::vector<std::string>(), std::vector<TypeNode*>(), return_type,
                             new BlockNode({}));
    COMPLETE_TEST;

}


TEST(parser_test, function_with_params_empty_body) {
    std::string text = "fun foo(x:List[List[Integer]])->Integer{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    auto expected_node = FUN("foo", { "x" }, { COMPLEX_TYPE }, TYPE("Integer", {}), new BlockNode({}));
    COMPLETE_TEST;

}

TEST(parser_test, function_with_params_and_body) {
    std::string text = "fun foo(x:List[List[Integer]])->String{x=a+b; x = y;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    auto expected_node = FUN("foo", { "x" }, { COMPLEX_TYPE }, TYPE("String", {}), BODY_NODE);
    COMPLETE_TEST;

}

TEST(parser_test, class_literal_exp) {
    std::string text = "Person{name, 27*32}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_id_or_class_literal();
    std::vector<Node*> init = {ID("name"), BIN(OpType::MUL, NUM(27), NUM(32))};
    auto expected_node = LIT_EXP("Person", init);
    COMPLETE_TEST;
}

TEST(parser_test, class_literal_fil) {
    std::string text = "Person{name: name, age: 27*32}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_id_or_class_literal();
    std::map<std::string, Node*> init = {{"name", ID("name")},
                                         {"age",  BIN(OpType::MUL, NUM(27), NUM(32))}};
    auto expected_node = LIT_FIL("Person", init);
    COMPLETE_TEST;
}

TEST(parser_test, class_literal_empty_ok) {
    std::string text = "Person{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_id_or_class_literal();
    std::vector<Node*> init = {};
    auto expected_node = LIT_EXP("Person", init);
    COMPLETE_TEST;
}

TEST(parser_test, class_literal_error_1) {
    std::string text = "Person{name: \"John\", age}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    try {
        Node* node = parser.parse_id_or_class_literal();
        FAIL() << "Didn't throw UnexpectedToken";
    } catch (const UnexpectedToken& e) {

    }
}

TEST(parser_test, class_literal_error_2) {
    std::string text = "Person{name, age: 32}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    try {
        Node* node = parser.parse_id_or_class_literal();
        FAIL() << "Didn't throw UnexpectedToken";
    } catch (const UnexpectedToken& e) {

    }
}

TEST(parser_test, function_with_params_return_type_and_body) {
    std::string text = FUN_FOO_STRING;
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    auto expected_node = FUN_FOO_NODE;
    COMPLETE_TEST;

}

TEST(parser_test, class_foo_empty) {
    std::string text = "struct Foo{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    StructNode* node = parser.parse_struct_definition();
    std::vector<std::pair<std::string, TypeNode*>> fields;
    auto expected_node = CLS("Foo", std::vector<std::string>(), fields);
    COMPLETE_TEST;

}

TEST(parser_test, class_foo_with_fields) {
    std::string text = "struct Foo{ x: String; y: Integer;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    StructNode* node = parser.parse_struct_definition();
    StructFields fields;
    fields.push_back(FieldInfo("x", T_STRING));
    fields.push_back(FieldInfo("y", T_INT));
    auto expected_node = CLS("Foo", {}, fields);
    COMPLETE_TEST;

}

TEST(parser_test, struct_literal_with_names) {
    std::string text = "Foo{ x: 27, y: 9}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    std::map<std::string, Node*> fields;
    fields["x"] = NUM(27);
    fields["y"] = NUM(9);
    auto expected_node = new ClassLiteralFieldNode("Foo", fields);
    COMPLETE_TEST;
}

TEST(parser_test, struct_literal_without_names) {
    std::string text = "Foo{9,27}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    std::vector<Node*> fields;
    fields.push_back(NUM(9));
    fields.push_back(NUM(27));
    auto expected_node = new ClassLiteralExpressionNode("Foo", fields);
    COMPLETE_TEST;
}


TEST(parser_test, template_class_foo_empty) {
    std::string text = "struct Foo[T, X]{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    StructNode* node = parser.parse_struct_definition();
    std::vector<std::string> params = {"T", "X"};
    StructFields fields;
    StructNode* expected_node = CLS("Foo", params, fields);
    COMPLETE_TEST;

}

TEST(parser_test, class_foo_with_fields_and_method) {
    std::string complete_foo_class_string = "struct Foo{ x: String;  y: Integer;}";
    StructFields fields;
    fields.push_back(FieldInfo("x", T_STRING));
    fields.push_back(FieldInfo("y", T_INT));
    StructNode* complete_foo_class_node = CLS("Foo", {}, fields);
    std::string text = complete_foo_class_string;
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    StructNode* node = parser.parse_struct_definition();
    auto expected_node = complete_foo_class_node;
    COMPLETE_TEST;

}

TEST(parser_test, simple_member) {
    std::string text = "a.b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = MEM(ID("a"), "b");
    COMPLETE_TEST;

}

TEST(parser_test, simple_id) {
    std::string text = "a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = ID("a");
    COMPLETE_TEST;

}

TEST(parser_test, simple_call) {
    std::string text = "a()";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = CALL(ID("a"), {});
    COMPLETE_TEST;

}

TEST(parser_test, simple_subscript) {
    std::string text = "a[1]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = SUB(ID("a"), NUM(1));
    COMPLETE_TEST;

}

TEST(parser_test, literal_number_expression) {
    std::string text = "7";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = NUM(7);
    COMPLETE_TEST;

}

TEST(parser_test, simple_parenthesized_expression) {
    std::string text = "(1)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = NUM(1);
    COMPLETE_TEST;

}

TEST(parser_test, plus_parenthesized_expression) {
    std::string text = "(1+a)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = BIN(OpType::ADD, NUM(1), ID("a"));
    COMPLETE_TEST;

}

TEST(parser_test, more_complex_expression) {
    std::string text = "(1+a)+b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = BIN(OpType::ADD, BIN(OpType::ADD, NUM(1), ID("a")), ID("b"));
    COMPLETE_TEST;

}

TEST(parser_test, more_complex_expression_2) {
    std::string text = "b*(1+a)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = BIN(OpType::MUL, ID("b"), BIN(OpType::ADD, NUM(1), ID("a")));
    COMPLETE_TEST;

}

TEST(parser_test, plus_expression) {
    std::string text = "1+a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = BIN(OpType::ADD, NUM(1), ID("a"));
    COMPLETE_TEST;

}

TEST(parser_test, minus_expression) {
    std::string text = "a-1";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = BIN(OpType::SUB, ID("a"), NUM(1));
    COMPLETE_TEST;

}

TEST(parser_test, plus_or_minus_with_multiple_terms_expression) {
    std::string text = "a+b-c";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = BIN(OpType::SUB, BIN(OpType::ADD, ID("a"), ID("b")), ID("c"));
    COMPLETE_TEST;

}

TEST(parser_test, mul_or_div_with_multiple_factors_expression) {
    std::string text = "a/b*c";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = BIN(OpType::MUL, BIN(OpType::DIV, ID("a"), ID("b")), ID("c"));
    COMPLETE_TEST;

}

TEST(parser_test, times_expression) {
    std::string text = "foo*bar";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_mul_or_div_expression();
    Node* expected_node = BIN(OpType::MUL, ID("foo"), ID("bar"));
    COMPLETE_TEST;

}

TEST(parser_test, boolean_true) {
    std::string text = "true";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = BOOL(true);
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 3);
    COMPLETE_TEST;
}

TEST(parser_test, boolean_false) {
    std::string text = "false";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = BOOL(false);
    COMPLETE_TEST;
}

TEST(parser_test, for_loop_1) {
    std::string text = "for(e:l){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_for_loop();
    Node* expected_node = FOR("e", ID("l"), new BlockNode({}));
    COMPLETE_TEST;
}

TEST(parser_test, while_loop_1) {
    std::string text = "while(true){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_while_loop();
    Node* expected_node = WHILE(BOOL(true), new BlockNode({}));
    COMPLETE_TEST;
}

TEST(parser_test, while_loop_common) {
    std::string text = "while(true){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_common_statement();
    Node* expected_node = WHILE(BOOL(true), new BlockNode({}));
    COMPLETE_TEST;
}

TEST(parser_test, for_loop_2) {
    std::string text = "for(e:[4,5,6]){print(e);}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_for_loop();
    VectorOfNodes list = {NUM(4), NUM(5), NUM(6)};
    BlockNode* body = new BlockNode({CALL(ID("print"), { ID("e") })});
    Node* expected_node = FOR("e", LST(list), body);
    COMPLETE_TEST;
}

TEST(parser_test, for_loop_3) {
    std::string text = "for(e:[4,5,6]){print(e);}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_top_level_statement();
    VectorOfNodes list = {NUM(4), NUM(5), NUM(6)};
    BlockNode* body = new BlockNode({CALL(ID("print"), { ID("e") })});
    Node* expected_node = FOR("e", LST(list), body);
    COMPLETE_TEST;
}

TEST(parser_test, div_expression) {
    std::string text = "foo/1";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_mul_or_div_expression();
    Node* expected_node = BIN(OpType::DIV, ID("foo"), NUM(1));
    COMPLETE_TEST;
}

TEST(parser_test, parse_list_empty) {
    std::string text = "[]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    VectorOfNodes list;
    Node* expected_node = LST(list);
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 1);
    COMPLETE_TEST;
}

TEST(parser_test, parse_xxx) {
    std::string text = "y[2]+x[7] + 43";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    VectorOfNodes list;
    Node* expected_node = BIN(OpType::ADD, BIN(OpType::ADD, SUB(ID("y"),NUM(2)),SUB(ID("x"),NUM(7))), NUM(43));
//    EXPECT_EQ(node->start, 0);
//    EXPECT_EQ(node->end, 1);
    COMPLETE_TEST;
}

TEST(parser_test, parse_list_one_element) {
    std::string text = "[13]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    VectorOfNodes list = {NUM(13)};
    Node* expected_node = LST(list);
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 3);
    COMPLETE_TEST;
}

TEST(parser_test, parse_none) {
    std::string text = "none";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new NoneNode();
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 3);
    COMPLETE_TEST;
}

TEST(parser_test, parse_list_multiple_elements) {
    std::string text = "[23,17,64]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    VectorOfNodes list = {NUM(23), NUM(17), NUM(64)};
    Node* expected_node = LST(list);
    EXPECT_EQ(node->start, 0);
    EXPECT_EQ(node->end, 9);
    COMPLETE_TEST;
}

TEST(parser_test, parse_top_level) {
    std::string text = "var x = 9*7;";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_top_level_statement();
    Node* expected_node = DECL("x", nullptr, BIN(OpType::MUL, NUM(9), NUM(7)));
    COMPLETE_TEST;

}

TEST(parser_test, complex_div_expression) {
    std::string text = "foo/(1)+a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = BIN(OpType::ADD, BIN(OpType::DIV, ID("foo"), NUM(1)), ID("a"));
    COMPLETE_TEST;

}

TEST(parser_test, complex_expression) {
    std::string text = "1-(7)*8-(9*(1-3)/7-8+4)+8";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = BIN(OpType::ADD, BIN(OpType::SUB, BIN(OpType::SUB, NUM(1),
                                                                BIN(OpType::MUL, NUM(7),
                                                                    NUM(8))),
                                               BIN(OpType::ADD, BIN(OpType::SUB,
                                                                    BIN(OpType::DIV,
                                                                        BIN(OpType::MUL,
                                                                            NUM(9),
                                                                            BIN(OpType::SUB,
                                                                                NUM(1),
                                                                                NUM(3))),
                                                                        NUM(7)), NUM(8)),
                                                   NUM(4))), NUM(8));
    COMPLETE_TEST;

}


TEST(parser_test, complex_chain) {
    std::string text = "a[1][b].c(1,d[5][0].e).f.g[h][2]()[3][5].i";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node =
            MEM(
                    SUB(
                            SUB(
                                    CALL(
                                            SUB(
                                                    SUB(
                                                            MEM(
                                                                    MEM(
                                                                            CALL(
                                                                                    MEM(
                                                                                            SUB(
                                                                                                    SUB(
                                                                                                            ID("a"),
                                                                                                            NUM(1)),
                                                                                                    ID("b")),
                                                                                            "c"),
                                                                                    std::vector<Node*>({NUM(1), MEM(
                                                                                            SUB(SUB(
                                                                                                    ID("d"),
                                                                                                    NUM(5)),
                                                                                                NUM(0)),
                                                                                            "e")})), "f"), "g"),
                                                            ID("h")),
                                                    NUM(2)), {}), NUM(3)), NUM(5)),
                    "i");
    COMPLETE_TEST;

}


TEST(parser_test, super_expression) {
    std::string text = "1-(a*c()[0]+7/d.a.x(7))*v*c/a+v.x.y[0][1][a+c*7](4,1,b+c)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = BIN(OpType::ADD,
                              BIN(OpType::SUB, NUM(1), BIN(OpType::DIV, BIN(OpType::MUL,
                                                                            BIN(OpType::MUL,
                                                                                BIN(OpType::ADD,
                                                                                    BIN(OpType::MUL,
                                                                                        ID("a"),
                                                                                        SUB(CALL(
                                                                                                ID("c"),
                                                                                                {}),
                                                                                            NUM(
                                                                                                    0))),
                                                                                    BIN(OpType::DIV,
                                                                                        NUM(7),
                                                                                        CALL(MEM(
                                                                                                MEM(
                                                                                                        ID("d"),
                                                                                                        "a"),
                                                                                                "x"),
                                                                                             {NUM(7)}))),
                                                                                ID("v")),
                                                                            ID("c")),
                                                           ID("a"))), CALL(
            SUB(SUB(SUB(MEM(MEM(ID("v"), "x"), "y"), NUM(0)), NUM(1)),
                BIN(OpType::ADD, ID("a"), BIN(OpType::MUL, ID("c"), NUM(7)))),
            std::vector<Node*>({NUM(4), NUM(1), BIN(OpType::ADD, ID("b"), ID("c"))})));
    std::cerr <<  *node << std::endl;

    COMPLETE_TEST;
}