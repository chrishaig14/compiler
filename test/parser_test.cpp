//
// Created by chris on 14/6/20.
//

#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <parser/UnexpectedToken.h>


#define BODY_NODE new BlockNode({N_ASN(N_ID("x"), N_BIN(OpType::ADD, N_ID("a"), N_ID("b"))),N_ASN(N_ID("x"), N_ID("y"))})
#define FUN_FOO_NODE FUN("foo", { "x" }, std::vector<TypeNode>({ COMPLEX_TYPE }), TYPE("List", {T_INT}), BODY_NODE)
#define FUN_FOO_STRING "fun foo(x:List[List[Integer]])->List[Integer]{x=a+b; x = y;}"
#define COMPLEX_TYPE T_LIST(T_LIST(T_INT))

#define COMPLETE_TEST  EXPECT_EQ(node, expected_node);
#define EXPECT_NOT_EQUAL EXPECT_FALSE(node->equal(expected_node)); delete node; delete expected_node;

TEST(parser_test, a_plus_b) {
    std::string text = "a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_add_or_sub_expression();
    NodeContainer expected_node = NodeContainer(
            N_BIN(OpType::ADD, NodeContainer(N_ID("a")), NodeContainer(N_ID("b"))));
    EXPECT_EQ(node.ntype, expected_node.ntype);
    EXPECT_EQ(node.binop(), expected_node.binop());
}

bool operator==(const NodeContainer& a, const NodeContainer& b) {
    return a.ntype == b.ntype;
}

TEST(parser_test, ternary) {
    std::string text = "a?7:6";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_ternary();
    NodeContainer expected_node = N_TERNARY(N_ID("a"), N_NUM(7), N_NUM(6));
    EXPECT_EQ(node.ntype, expected_node.ntype);
    EXPECT_EQ(node.ternary(), expected_node.ternary());
}

TEST(parser_test, a_or_b) {
    std::string text = "a||b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_or_expression();
    NodeContainer expected_node = N_BIN(OpType::OR, N_ID("a"), N_ID("b"));
    EXPECT_EQ(node.ntype, expected_node.ntype);
    EXPECT_EQ(node.binop(), expected_node.binop());


}

TEST(parser_test, complex_expression_1) {
    std::string text = "a||s-c==7?8-9?7:4:10";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_TERNARY(
            N_BIN(OpType::OR, N_ID("a"), N_BIN(OpType::EQ, N_BIN(OpType::SUB, N_ID("s"), N_ID("c")), N_NUM(7))),
            N_TERNARY(N_BIN(OpType::SUB, N_NUM(8), N_NUM(9)), N_NUM(7), N_NUM(4)),
            N_NUM(10));
    EXPECT_EQ(node.ntype, expected_node.ntype);
    EXPECT_EQ(node.ternary(), expected_node.ternary());

}

TEST(parser_test, a_eq_b) {
    std::string text = "a==b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_BIN(OpType::EQ, N_ID("a"), N_ID("b"));


    COMPLETE_TEST;

}

TEST(parser_test, id) {
    std::string text = "foo";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_add_or_sub_expression();
    auto expected_node = N_ID("foo");


    COMPLETE_TEST;
}

TEST(parser_test, assign_x_y) {
    std::string text = "x = y";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_assignment_or_expression();
    NodeContainer expected_node = N_ASN(N_ID("x"), N_ID("y"));


    COMPLETE_TEST;


}

TEST(parser_test, assign_x_a_plus_b) {
    std::string text = "x = a+b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_assignment_or_expression();
    NodeContainer expected_node = N_ASN(N_ID("x"), N_BIN(OpType::ADD, N_ID("a"), N_ID("b")));


    COMPLETE_TEST;

}

TEST(parser_test, declare_x_no_type_no_exp_error) {
    std::string text = "var x";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    try {
        DeclarationNode node = parser.parse_variable_declaration();
        FAIL() << "Didn't throw  unexpected token error";
    } catch (...) {

    }
}

TEST(parser_test, declare_x_a_plus_b) {
    std::string text = "var x = a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    DeclarationNode node = parser.parse_variable_declaration();
    DeclarationNode expected_node = DeclarationNode("x", nullptr, N_BIN(OpType::ADD, N_ID("a"), N_ID("b")));
    EXPECT_EQ(node, expected_node);
}

TEST(parser_test, empty_block) {
    std::string text = "{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode node = parser.parse_possibly_empty_block();


    EXPECT_EQ(node.nodes.size(), 0);
}

TEST(parser_test, non_empty_block) {
    std::string text = "{x = a + b; x = y;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode node = parser.parse_possibly_empty_block();
    EXPECT_EQ(node.nodes.size(), 2);
    auto expected_node_0 = N_ASN(N_ID("x"), N_BIN(OpType::ADD, N_ID("a"), N_ID("b")));
    EXPECT_TRUE(node.nodes[0] == expected_node_0);
    auto expected_node_1 = N_ASN(N_ID("x"), N_ID("y"));
    EXPECT_TRUE(node.nodes[1] == (expected_node_1));
}

TEST(parser_test, if_x_empty_then) {
    std::string text = "if(x){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    IfNode node = parser.parse_if();
//    auto expected_node = IF(N_ID("x"), new BlockNode({}));
//    COMPLETE_TEST;
}

TEST(parser_test, if_with_else) {
    std::string text = "if(x){}else{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    IfNode node = parser.parse_if();
//    auto expected_node = IFELSE(N_ID("x"), new BlockNode({}), new BlockNode({}));
//    COMPLETE_TEST;
}

TEST(parser_test, if_x_non_empty_then) {
    std::string text = "if(x){x=a+b; x = y;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    IfNode node = parser.parse_if();
    auto st_1 = N_ASN(N_ID("x"), N_BIN(OpType::ADD, N_ID("a"), N_ID("b")));
    auto st_2 = N_ASN(N_ID("x"), N_ID("y"));
//    auto expected_node = IF(N_ID("x"), new BlockNode({st_1, st_2}));
//    COMPLETE_TEST;

}

TEST(parser_test, simple_type) {
    std::string text = "String";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode node = parser.parse_type_node();
    auto expected_node = TypeNode(T_STRING);
//    COMPLETE_TEST;
    EXPECT_EQ(node, expected_node);
}

TEST(parser_test, template_type) {
    std::string text = "String[Integer]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode node = parser.parse_type_node();
    auto expected_node = TypeNode(TYPE("String", std::vector<TypeNode>{OBJECT_TYPE("Integer", {})}));
//    COMPLETE_TEST;
    EXPECT_EQ(node, expected_node);
}

TEST(parser_test, fun_empty) {
    std::string text = "fun() ->String";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode node = parser.parse_type_node();
    auto expected_node = TypeNode(FUNCTION_TYPE({}, T_STRING));
    EXPECT_EQ(node, expected_node);

//    COMPLETE_TEST;
}

TEST(parser_test, fun_full) {
    std::string text = "fun(List[String],fun(String)->Integer) -> List[Integer]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode node = parser.parse_type_node();
    auto t = {TypeNode(T_LIST(T_STRING)), TypeNode(FUNCTION_TYPE({ T_STRING }, T_INT))};
    auto expected_node = TypeNode(FUNCTION_TYPE(t, TypeNode(T_LIST(T_INT))));
    EXPECT_EQ(node, expected_node);

//    COMPLETE_TEST;
}


TEST(parser_test, complex_template_type) {
    std::string text = "String[Integer, List[String]]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode node = parser.parse_type_node();
    auto expected_node = TypeNode(TYPE("String", std::vector<TypeNode>({T_INT, T_LIST({T_STRING})})));
    EXPECT_EQ(node, expected_node);

}

TEST(parser_test, decl_with_type_and_value) {
    std::string text = "var x:String[Integer, List[String]] = a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    DeclarationNode node = parser.parse_variable_declaration();
    auto expected_node = DeclarationNode("x", new TypeNode(
                                                 TYPE("String", std::vector<TypeNode>({T_INT, T_LIST({T_STRING})}))),
                                         N_BIN(OpType::ADD, N_ID("a"), N_ID("b")));
    EXPECT_EQ(node, expected_node);

//    COMPLETE_TEST;
}

TEST(parser_test, function_no_params_empty_body) {
    std::string text = "fun foo()->String{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode node = parser.parse_function_definition();
    auto return_type = TYPE("String", {});
    BlockNode b;
    auto expected_node = FunctionNode("foo", std::vector<std::string>(), std::vector<TypeNode>(), return_type, b);
//    COMPLETE_TEST;
    EXPECT_EQ(node, expected_node);


}


TEST(parser_test, function_with_params_empty_body) {
    std::string text = "fun foo(x:List[List[Integer]])->Integer{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode node = parser.parse_function_definition();
    BlockNode b;
    auto expected_node = FunctionNode("foo", {"x"}, {COMPLEX_TYPE}, TYPE("Integer", {}), b);
    EXPECT_EQ(node, expected_node);
}

TEST(parser_test, function_with_params_and_body) {
    std::string text = "fun foo(x:List[List[Integer]])->String{x=a+b; x = y;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode node = parser.parse_function_definition();
    BlockNode b({N_ASN(N_ID("x"), N_BIN(OpType::ADD, N_ID("a"), N_ID("b"))), N_ASN(N_ID("x"), N_ID("y"))});
    auto expected_node = FunctionNode("foo", {"x"}, {COMPLEX_TYPE}, TYPE("String", {}), b);
    EXPECT_EQ(node, expected_node);
}

TEST(parser_test, class_literal_fil) {
    std::string text = "#Person{name: name, age: 27*32}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_factor();
    std::map<std::string, NodeContainer> init = {{"name", N_ID("name")},
                                                 {"age",  N_BIN(OpType::MUL, N_NUM(27), N_NUM(32))}};
    auto expected_node = LIT_FIL(OBJECT_TYPE("Person", {}), init);
//    COMPLETE_TEST;
}

TEST(parser_test, class_literal_empty_ok) {
    std::string text = "#Person{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_factor();
    std::vector<NodeContainer> init = {};
    auto expected_node = LIT_EXP(OBJECT_TYPE("Person", {}), init);
//    COMPLETE_TEST;
}

TEST(parser_test, class_literal_error_2) {
    std::string text = "#Person{name, age: 32}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    try {
        NodeContainer node = parser.parse_factor();
        FAIL() << "Didn't throw UnexpectedToken";
    } catch (const UnexpectedToken& e) {

    }
}

TEST(parser_test, function_with_params_return_type_and_body) {
    std::string text = FUN_FOO_STRING;
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode node = parser.parse_function_definition();
//    auto expected_node = FUN_FOO_NODE;
//    COMPLETE_TEST;

}

TEST(parser_test, class_foo_empty) {
//    std::string text = "struct Foo{}";
//    Scanner scanner(text);
//    std::vector<Token> tokens = scanner.scan_all();
//    Parser parser(tokens);
//    StructNode node = parser.parse_struct_definition();
//    std::vector<std::pair<std::string, TypeNode>> fields;
//    auto expected_node = CLS("Foo", std::vector<std::string>(), fields);
//    COMPLETE_TEST;

}

TEST(parser_test, class_foo_with_fields) {
//    std::string text = "struct Foo{ x: String; y: Integer;}";
//    Scanner scanner(text);
//    std::vector<Token> tokens = scanner.scan_all();
//    Parser parser(tokens);
//    StructNode node = parser.parse_struct_definition();
//    StructFields fields;
//    fields.push_back(FieldInfo("x", T_STRING));
//    fields.push_back(FieldInfo("y", T_INT));
//    auto expected_node = CLS("Foo", {}, fields);
//    COMPLETE_TEST;

}

TEST(parser_test, struct_literal_with_names) {
    std::string text = "#Foo{ x: 27, y: 9}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    std::map<std::string, NodeContainer> fields;
    fields["x"] = N_NUM(27);
    fields["y"] = N_NUM(9);
    auto expected_node = NodeContainer(new ClassLiteralFieldNode(OBJECT_TYPE("Foo", {}), fields));
    COMPLETE_TEST;
}

TEST(parser_test, struct_literal_without_names) {
    std::string text = "#Foo{9,27}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    std::vector<NodeContainer> fields;
    fields.push_back(N_NUM(9));
    fields.push_back(N_NUM(27));
    auto expected_node = NodeContainer(new ClassLiteralExpressionNode(OBJECT_TYPE("Foo", {}), fields));
    COMPLETE_TEST;
}


TEST(parser_test, template_class_foo_empty) {
    std::string text = "struct Foo[T, X]{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
//    StructNode node = parser.parse_struct_definition();
//    std::vector<std::string> params = {"T", "X"};
//    StructFields fields;
//    StructNode expected_node = CLS("Foo", params, fields);
//    COMPLETE_TEST;

}

TEST(parser_test, class_foo_with_fields_and_method) {
    std::string complete_foo_class_string = "struct Foo{ x: String;  y: Integer;}";
    StructFields fields;
//    fields.push_back(FieldInfo("x", T_STRING));
//    fields.push_back(FieldInfo("y", T_INT));
//    StructNode complete_foo_class_node = CLS("Foo", {}, fields);
//    std::string text = complete_foo_class_string;
//    Scanner scanner(text);
//    std::vector<Token> tokens = scanner.scan_all();
//    Parser parser(tokens);
////    StructNode node = parser.parse_struct_definition();
////    auto expected_node = complete_foo_class_node;
////    COMPLETE_TEST;

}

TEST(parser_test, simple_member) {
    std::string text = "a.b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_MEM(N_ID("a"), "b");
    COMPLETE_TEST;

}

TEST(parser_test, simple_id) {
    std::string text = "a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_ID("a");
    COMPLETE_TEST;

}

TEST(parser_test, simple_call) {
    std::string text = "a()";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_CALL(N_ID("a"), {});
    COMPLETE_TEST;

}

TEST(parser_test, simple_subscript) {
    std::string text = "a[1]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_SUB(N_ID("a"), { N_NUM(1) });
    COMPLETE_TEST;
}

TEST(parser_test, literal_number_expression) {
    std::string text = "7";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_NUM(7);
    COMPLETE_TEST;

}

TEST(parser_test, simple_parenthesized_expression) {
    std::string text = "(1)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_NUM(1);
    COMPLETE_TEST;

}

TEST(parser_test, test_now_1) {
    std::string text = "x";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_ID("x");
    COMPLETE_TEST;
}

TEST(parser_test, test_now_2) {
    std::string text = "x[7]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_SUB(N_ID("x"), { N_NUM(7) });
    COMPLETE_TEST;
}

TEST(parser_test, test_now_3_should_fail) {
    std::string text = "x[7]{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_SUB(N_ID("x"), { N_NUM(7) });
    COMPLETE_TEST;
}

TEST(parser_test, test_now_3) {
    std::string text = "#x[y]{v:7}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    std::map<std::string, NodeContainer> fields;
    fields["v"] = N_NUM(7);
//    NodeContainer expected_node = LIT_FIL(TYPE("x", {TYPE("y", {})}), fields);
//    COMPLETE_TEST;
}

TEST(parser_test, test_now_4) {
    std::string text = "#x[y]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    try {
        parser.parse_expression();
        FAIL() << "Expected exception!";
    } catch (const UnexpectedToken& e) {

    }
}

TEST(parser_test, test_now_8) {
    std::string text = "#x[y]{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    std::map<std::string, NodeContainer> fields;
//    NodeContainer expected_node = LIT_FIL(TYPE("x", {TYPE("y", {})}), fields);
//    COMPLETE_TEST;
//    EXPECT_EQ(node, expected_node);

}

TEST(parser_test, plus_parenthesized_expression) {
    std::string text = "(1+a)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_BIN(OpType::ADD, N_NUM(1), N_ID("a"));
    COMPLETE_TEST;

}

TEST(parser_test, more_complex_expression) {
    std::string text = "(1+a)+b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_BIN(OpType::ADD, N_BIN(OpType::ADD, N_NUM(1), N_ID("a")),
                                        N_ID("b"));
    COMPLETE_TEST;

}

TEST(parser_test, more_complex_expression_2) {
    std::string text = "b*(1+a)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_BIN(OpType::MUL, N_ID("b"),
                                        N_BIN(OpType::ADD, N_NUM(1), N_ID("a")));
    COMPLETE_TEST;

}

TEST(parser_test, plus_expression) {
    std::string text = "1+a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_add_or_sub_expression();
    NodeContainer expected_node = N_BIN(OpType::ADD, N_NUM(1), N_ID("a"));
    COMPLETE_TEST;

}

TEST(parser_test, minus_expression) {
    std::string text = "a-1";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_add_or_sub_expression();
    NodeContainer expected_node = N_BIN(OpType::SUB, N_ID("a"), N_NUM(1));
    COMPLETE_TEST;

}

TEST(parser_test, plus_or_minus_with_multiple_terms_expression) {
    std::string text = "a+b-c";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_add_or_sub_expression();
    NodeContainer expected_node = N_BIN(OpType::SUB,
                                        N_BIN(OpType::ADD, N_ID("a"), N_ID("b")),
                                        N_ID("c"));
    COMPLETE_TEST;

}

TEST(parser_test, mul_or_div_with_multiple_factors_expression) {
    std::string text = "a/b*c";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_add_or_sub_expression();
    NodeContainer expected_node = N_BIN(OpType::MUL,
                                        N_BIN(OpType::DIV, N_ID("a"), N_ID("b")),
                                        N_ID("c"));
    COMPLETE_TEST;

}

TEST(parser_test, times_expression) {
    std::string text = "foo*bar";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_mul_div_or_mod_expression();
    NodeContainer expected_node = N_BIN(OpType::MUL, N_ID("foo"), N_ID("bar"));
    COMPLETE_TEST;

}

TEST(parser_test, boolean_true) {
    std::string text = "true";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_BOOL(true);


    COMPLETE_TEST;
}

TEST(parser_test, boolean_false) {
    std::string text = "false";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_BOOL(false);
    COMPLETE_TEST;
}

TEST(parser_test, for_loop_1) {
    std::string text = "for(e@l){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ForNode node = parser.parse_for_loop();
    ForNode expected_node = ForNode("e", N_ID("l"), BlockNode());
    EXPECT_EQ(node, expected_node);
}

TEST(parser_test, while_loop_1) {
    std::string text = "while(true){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    WhileNode node = parser.parse_while_loop();
    WhileNode expected_node = WhileNode(N_BOOL(true), BlockNode());
    EXPECT_EQ(node, expected_node);
}

TEST(parser_test, while_loop_common) {
    std::string text = "while(true){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    WhileNode node = parser.parse_while_loop();
    WhileNode expected_node = WhileNode(N_BOOL(true), BlockNode());
    EXPECT_EQ(node, expected_node);
}

TEST(parser_test, for_loop_2) {
    std::string text = "for(e@[4,5,6]){print(e);}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ForNode node = parser.parse_for_loop();
    VectorOfNodes list = {N_NUM(4), N_NUM(5), N_NUM(6)};
    BlockNode body = BlockNode({N_CALL(N_ID("print"), { N_ID("e") })});
    ForNode expected_node = ForNode("e", N_LST(list), body);
    COMPLETE_TEST;
}

TEST(parser_test, for_loop_3) {
    std::string text = "for(e @[4,5,6]){print(e);}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_top_level_statement();
    VectorOfNodes list = {N_NUM(4), N_NUM(5), N_NUM(6)};
    BlockNode body = BlockNode({N_CALL(N_ID("print"), { N_ID("e") })});
    NodeContainer expected_node = N_FOR("e", N_LST(list), body);
    COMPLETE_TEST;
}

TEST(parser_test, div_expression) {
    std::string text = "foo/1";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_mul_div_or_mod_expression();
    NodeContainer expected_node = N_BIN(OpType::DIV, N_ID("foo"), N_NUM(1));
    COMPLETE_TEST;
}

TEST(parser_test, parse_list_empty_no_type_throws_error) {
    std::string text = "[]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    try {
        NodeContainer node = parser.parse_expression();
        FAIL() << "Did not throw an error";
    } catch (...) {

    }
}

TEST(parser_test, parse_xxx) {
    std::string text = "y[2]+x[7] + 43";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    VectorOfNodes list;
    NodeContainer expected_node = N_BIN(OpType::ADD,
                                        N_BIN(OpType::ADD, N_SUB(N_ID("y"), {N_NUM(2)}),
                                              N_SUB(N_ID("x"), {N_NUM(7)})), N_NUM(43));
//    
//    
    COMPLETE_TEST;
}

TEST(parser_test, parse_empty_list) {
    std::string text = "[]::Integer";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    VectorOfNodes list;
    NodeContainer expected_node = NodeContainer(new EmptyListNode(T_INT));
//    
//    
    COMPLETE_TEST;
}

TEST(parser_test, parse_list_one_element) {
    std::string text = "[13]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    VectorOfNodes list = {N_NUM(13)};
    NodeContainer expected_node = N_LST(list);


    COMPLETE_TEST;
}

TEST(parser_test, parse_none) {
    std::string text = "none";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = NodeContainer(new NoneNode());
    COMPLETE_TEST;
}

TEST(parser_test, parse_list_multiple_elements) {
    std::string text = "[23,17,64]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    VectorOfNodes list = {N_NUM(23), N_NUM(17), N_NUM(64)};
    NodeContainer expected_node = N_LST(list);


    COMPLETE_TEST;
}

TEST(parser_test, parse_top_level) {
    std::string text = "var x = 9*7;";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_top_level_statement();
    NodeContainer expected_node = N_DECL("x", nullptr, N_BIN(OpType::MUL, N_NUM(9), N_NUM(7)));
    COMPLETE_TEST;

}

TEST(parser_test, complex_div_expression) {
    std::string text = "foo/(1)+a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_add_or_sub_expression();
    NodeContainer expected_node = N_BIN(OpType::ADD, N_BIN(OpType::DIV, N_ID("foo"), N_NUM(1)),
                                        N_ID("a"));
    COMPLETE_TEST;

}

TEST(parser_test, complex_expression) {
    std::string text = "1-(7)*8-(9*(1-3)/7-8+4)+8";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    NodeContainer expected_node = N_BIN(OpType::ADD,
                                        N_BIN(OpType::SUB, N_BIN(OpType::SUB, N_NUM(1),
                                                                 N_BIN(OpType::MUL,
                                                                       N_NUM(7),
                                                                       N_NUM(8))),
                                              N_BIN(OpType::ADD, N_BIN(OpType::SUB,
                                                                       N_BIN(
                                                                               OpType::DIV,
                                                                               N_BIN(
                                                                                       OpType::MUL,
                                                                                       N_NUM(9),
                                                                                       N_BIN(
                                                                                               OpType::SUB,
                                                                                               N_NUM(1),
                                                                                               N_NUM(3))),
                                                                               N_NUM(7)),
                                                                       N_NUM(8)),
                                                    N_NUM(4))), N_NUM(8));
    COMPLETE_TEST;

}


TEST(parser_test, complex_chain) {
    std::string text = "a[1][b].c(d[5][0].e).f.g[h][2]()[3][5].i";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    NodeContainer node = parser.parse_expression();
    // a[1][b].c(d[5][0].e
    auto call = N_CALL(N_MEM(N_SUB(N_SUB(N_ID("a"), {N_NUM(1)}), {N_ID("b")}), "c"),
                       { N_MEM(N_SUB(N_SUB(N_ID("d"), {N_NUM(5)}), {N_NUM(0)}), "e") });
    NodeContainer expected_node = N_MEM(
            N_SUB(N_SUB(N_CALL(N_SUB(N_SUB(N_MEM(N_MEM(call, "f"), "g"), {N_ID("h")}), {N_NUM(2)}), {}), {N_NUM(3)}),
                  {N_NUM(5)}), "i");
    EXPECT_EQ(node, expected_node);

}


//TEST(parser_test, super_expression) {
//    std::string text = "1-(a*c()[0]+7/d.a.x(7))*v*c/a+v.x.y[0][1][a+c*7](4,1,b+c)";
//    Scanner scanner(text);
//    std::vector<Token> tokens = scanner.scan_all();
//    Parser parser(tokens);
//    NodeContainer node = parser.parse_expression();
//    NodeContainer expected_node = N_BIN(OpType::ADD,
//                              N_BIN(OpType::SUB, N_NUM(1), N_BIN(OpType::DIV, N_BIN(OpType::MUL,
//                                                                            N_BIN(OpType::MUL,
//                                                                                N_BIN(OpType::ADD,
//                                                                                    N_BIN(OpType::MUL,
//                                                                                        N_ID("a"),
//                                                                                        N_SUB(N_CALL(
//                                                                                                N_ID("c"),
//                                                                                                {}),
//                                                                                            N_NUM(
//                                                                                                    0))),
//                                                                                    N_BIN(OpType::DIV,
//                                                                                        N_NUM(7),
//                                                                                        N_CALL(MEM(
//                                                                                                MEM(
//                                                                                                        N_ID("d"),
//                                                                                                        "a"),
//                                                                                                "x"),
//                                                                                             {N_NUM(7)}))),
//                                                                                N_ID("v")),
//                                                                            N_ID("c")),
//                                                           N_ID("a"))), N_CALL(
//            N_SUB(N_SUB(N_SUB(MEM(MEM(N_ID("v"), "x"), "y"), N_NUM(0)), N_NUM(1)),
//                N_BIN(OpType::ADD, N_ID("a"), N_BIN(OpType::MUL, N_ID("c"), N_NUM(7)))),
//            std::vector<NodeContainer>({N_NUM(4), N_NUM(1), N_BIN(OpType::ADD, N_ID("b"), N_ID("c"))})));
//    std::cerr << *node << std::endl;
//
//    COMPLETE_TEST;
//}
