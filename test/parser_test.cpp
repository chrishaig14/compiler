//
// Created by chris on 14/6/20.
//

#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <parser/UnexpectedToken.h>
#include <macros.h>


#define BODY_NODE new BlockNode({new AssignmentNode(new IdNode("x"), new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b"))),new AssignmentNode(new IdNode("x"), new IdNode("y"))})
#define FUN_FOO_NODE FUN("foo", { "x" }, VectorOfTypes({ COMPLEX_TYPE }), TYPE("List", {T_INT}), BODY_NODE)
#define FUN_FOO_STRING "fun foo(x:List[List[Integer]])->List[Integer]{x=a+b; x = y;}"
#define COMPLEX_TYPE T_LIST(T_LIST(T_INT))

#define COMPLETE_TEST  EXPECT_EQ(*node, *expected_node);
#define EXPECT_NOT_EQUAL EXPECT_FALSE(node->equal(expected_node)); delete node; delete expected_node;

TEST(parser_test, a_plus_b) {
    std::string text = "a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = (
            new BinopNode(OpType::ADD, (new IdNode("a")), (new IdNode("b"))));
    EXPECT_EQ(node->ntype, expected_node->ntype);
    EXPECT_EQ(node->binop(), expected_node->binop());
}

TEST(parser_test, ternary) {
    std::string text = "a?7:6";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_ternary();
    Node* expected_node = new TernaryNode(new IdNode("a"), new NumberNode(7), new NumberNode(6));
    EXPECT_EQ(*node, *expected_node);
}

TEST(parser_test, a_or_b) {
    std::string text = "a||b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_or_expression();
    Node* expected_node = new BinopNode(OpType::OR, new IdNode("a"), new IdNode("b"));
    EXPECT_EQ(node->ntype, expected_node->ntype);
    EXPECT_EQ(node->binop(), expected_node->binop());


}

TEST(parser_test, complex_expression_1) {
    std::string text = "a||s-c==7?8-9?7:4:10";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new TernaryNode(
            new BinopNode(OpType::OR, new IdNode("a"),
                          new BinopNode(OpType::EQ, new BinopNode(OpType::SUB, new IdNode("s"), new IdNode("c")),
                                        new NumberNode(7))),
            new TernaryNode(new BinopNode(OpType::SUB, new NumberNode(8), new NumberNode(9)), new NumberNode(7),
                            new NumberNode(4)),
            new NumberNode(10));
    EXPECT_EQ(node->ntype, expected_node->ntype);
    EXPECT_EQ(node->ternary(), expected_node->ternary());

}

TEST(parser_test, a_eq_b) {
    std::string text = "a==b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new BinopNode(OpType::EQ, new IdNode("a"), new IdNode("b"));


    COMPLETE_TEST;

}

TEST(parser_test, id) {
    std::string text = "foo";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    auto expected_node = new IdNode("foo");


    COMPLETE_TEST;
}

TEST(parser_test, assign_x_y) {
    std::string text = "x = y";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_assignment_or_expression();
    Node* expected_node = new AssignmentNode(new IdNode("x"), new IdNode("y"));


    COMPLETE_TEST;


}

TEST(parser_test, assign_x_a_plus_b) {
    std::string text = "x = a+b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_assignment_or_expression();
    Node* expected_node = new AssignmentNode(new IdNode("x"),
                                             new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));


    COMPLETE_TEST;

}

TEST(parser_test, declare_x_no_type_no_exp_error) {
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

TEST(parser_test, declare_x_a_plus_b) {
    std::string text = "var x = a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    DeclarationNode* node = parser.parse_variable_declaration();
    DeclarationNode* expected_node = new DeclarationNode("x", nullptr,
                                                         new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_EQ(*node, *expected_node);
}

TEST(parser_test, empty_block) {
    std::string text = "{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* node = parser.parse_possibly_empty_block();


    EXPECT_EQ(node->nodes.size(), 0);
}

TEST(parser_test, non_empty_block) {
    std::string text = "{x = a + b; x = y;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* node = parser.parse_possibly_empty_block();
    EXPECT_EQ(node->nodes.size(), 2);
    auto expected_node_0 = new AssignmentNode(new IdNode("x"),
                                              new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_EQ(*node->nodes[0], *expected_node_0);
    auto expected_node_1 = new AssignmentNode(new IdNode("x"), new IdNode("y"));
    EXPECT_EQ(*node->nodes[1], *(expected_node_1));
}

TEST(parser_test, if_x_empty_then) {
    std::string text = "if(x){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    IfNode* node = parser.parse_if();
    auto expected_node = new IfNode(new IdNode("x"), new BlockNode({}), {}, {});
    COMPLETE_TEST;
}

TEST(parser_test, if_with_else) {
    std::string text = "if(x){}else{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    IfNode* node = parser.parse_if();
    auto expected_node = new IfNode(new IdNode("x"), new BlockNode({}), {}, new BlockNode({}));
    COMPLETE_TEST;
}

TEST(parser_test, if_x_non_empty_then) {
    std::string text = "if(x){x=a+b; x = y;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    IfNode* node = parser.parse_if();
    auto st_1 = new AssignmentNode(new IdNode("x"), new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    auto st_2 = new AssignmentNode(new IdNode("x"), new IdNode("y"));
//    auto expected_node = IF(new IdNode("x"), new BlockNode({st_1, st_2}));
//    COMPLETE_TEST;

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
    auto expected_node = TYPE("String", VectorOfTypes{TYPE("Integer", {})});
//    COMPLETE_TEST;
    EXPECT_EQ(*node, *expected_node);
}

TEST(parser_test, fun_empty) {
    std::string text = "fun() ->String";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    auto expected_node = FUNCTION_TYPE({}, T_STRING);
    EXPECT_EQ(*node, *expected_node);

//    COMPLETE_TEST;
}

TEST(parser_test, fun_full) {
    std::string text = "fun(List[String],fun(String)->Integer) -> List[Integer]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    VectorOfTypes t = {T_LIST(T_STRING), FUNCTION_TYPE({ T_STRING }, T_INT)};
    auto expected_node = FUNCTION_TYPE(t, T_LIST(T_INT));
    EXPECT_EQ(*node, *expected_node);

//    COMPLETE_TEST;
}


TEST(parser_test, complex_template_type) {
    std::string text = "String[Integer, List[String]]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    TypeNode* node = parser.parse_type_node();
    auto expected_node = TYPE("String", VectorOfTypes({T_INT, T_LIST({T_STRING})}));
    EXPECT_EQ(*node, *expected_node);

}

TEST(parser_test, decl_with_type_and_value) {
    std::string text = "var x:String[Integer, List[String]] = a + b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    DeclarationNode* node = parser.parse_variable_declaration();
    auto expected_node = new DeclarationNode("x", TYPE("String", VectorOfTypes({T_INT, T_LIST({T_STRING})})),
                                             new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_EQ(*node, *expected_node);

//    COMPLETE_TEST;
}

TEST(parser_test, function_no_params_empty_body) {
    std::string text = "fun foo()->String{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    auto return_type = TYPE("String", {});
    BlockNode* b = new BlockNode({});
    auto expected_node = new FunctionNode("foo", std::vector<std::string>(), VectorOfTypes(), return_type, b);
//    COMPLETE_TEST;
    EXPECT_EQ(*node, *expected_node);


}


TEST(parser_test, function_with_params_empty_body) {
    std::string text = "fun foo(x:List[List[Integer]])->Integer{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    BlockNode* b = new BlockNode({});
    auto expected_node = new FunctionNode("foo", {"x"}, {COMPLEX_TYPE}, TYPE("Integer", {}), b);
    EXPECT_EQ(*node, *expected_node);
}

TEST(parser_test, function_with_params_and_body) {
    std::string text = "fun foo(x:List[List[Integer]])->String{x=a+b; x = y;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionNode* node = parser.parse_function_definition();
    BlockNode* b = new BlockNode(
            {new AssignmentNode(new IdNode("x"), new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b"))),
             new AssignmentNode(new IdNode("x"), new IdNode("y"))});
    auto expected_node = new FunctionNode("foo", {"x"}, {COMPLEX_TYPE}, TYPE("String", {}), b);
    EXPECT_EQ(*node, *expected_node);
}

TEST(parser_test, class_literal_fil) {
    std::string text = "#Person{name: name, age: 27*32}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_factor();
    std::map<std::string, Node*> init = {{"name", new IdNode("name")},
                                         {"age",  new BinopNode(OpType::MUL, new NumberNode(27), new NumberNode(32))}};
//    auto expected_node = LIT_FIL(TYPE("Person", {}), init);
//    COMPLETE_TEST;
}

TEST(parser_test, class_literal_empty_ok) {
    std::string text = "#Person{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_factor();
    VectorOfNodes init = {};
//    auto expected_node = LIT_EXP(TYPE("Person", {}), init);
//    COMPLETE_TEST;
}

TEST(parser_test, class_literal_error_2) {
    std::string text = "#Person{name, age: 32}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    try {
        Node* node = parser.parse_factor();
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
//    auto expected_node = FUN_FOO_NODE;
//    COMPLETE_TEST;

}

TEST(parser_test, class_foo_empty) {
//    std::string text = "struct Foo{}";
//    Scanner scanner(text);
//    std::vector<Token> tokens = scanner.scan_all();
//    Parser parser(tokens);
//    StructNode* node = parser.parse_struct_definition();
//    std::vector<std::pair<std::string, TypeNode*> fields;
//    auto expected_node = CLS("Foo", std::vector<std::string>(), fields);
//    COMPLETE_TEST;

}

TEST(parser_test, class_foo_with_fields) {
//    std::string text = "struct Foo{ x: String; y: Integer;}";
//    Scanner scanner(text);
//    std::vector<Token> tokens = scanner.scan_all();
//    Parser parser(tokens);
//    StructNode* node = parser.parse_struct_definition();
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
    Node* node = parser.parse_expression();
    std::map<std::string, Node*> fields;
    fields["x"] = new NumberNode(27);
    fields["y"] = new NumberNode(9);
//    auto expected_node = (new ClassLiteralFieldNode(TYPE("Foo", {}), fields));
//    COMPLETE_TEST;
}

TEST(parser_test, struct_literal_without_names) {
    std::string text = "#Foo{9,27}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    VectorOfNodes fields;
    fields.push_back(new NumberNode(9));
    fields.push_back(new NumberNode(27));
//    auto expected_node = (new ClassLiteralExpressionNode(TYPE("Foo", {}), fields));
//    COMPLETE_TEST;
}


TEST(parser_test, template_class_foo_empty) {
    std::string text = "struct Foo[T, X]{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
//    StructNode* node = parser.parse_struct_definition();
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
////    StructNode* node = parser.parse_struct_definition();
////    auto expected_node = complete_foo_class_node;
////    COMPLETE_TEST;

}

TEST(parser_test, simple_member) {
    std::string text = "a.b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new MemberNode(new IdNode("a"), "b");
    COMPLETE_TEST;

}

TEST(parser_test, simple_id) {
    std::string text = "a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new IdNode("a");
    COMPLETE_TEST;

}

TEST(parser_test, simple_call) {
    std::string text = "a()";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new CallNode(new IdNode("a"), {});
    COMPLETE_TEST;

}

TEST(parser_test, simple_subscript) {
    std::string text = "a[1]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new SubscriptNode(new IdNode("a"), {new NumberNode(1)});
    COMPLETE_TEST;
}

TEST(parser_test, literal_number_expression) {
    std::string text = "7";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new NumberNode(7);
    COMPLETE_TEST;

}

TEST(parser_test, simple_parenthesized_expression) {
    std::string text = "(1)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new NumberNode(1);
    COMPLETE_TEST;

}

TEST(parser_test, test_now_1) {
    std::string text = "x";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new IdNode("x");
    COMPLETE_TEST;
}

TEST(parser_test, test_now_2) {
    std::string text = "x[7]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new SubscriptNode(new IdNode("x"), {new NumberNode(7)});
    COMPLETE_TEST;
}

TEST(parser_test, test_now_3_should_fail) {
    std::string text = "x[7]{}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new SubscriptNode(new IdNode("x"), {new NumberNode(7)});
    COMPLETE_TEST;
}

TEST(parser_test, test_now_3) {
    std::string text = "#x[y]{v:7}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    std::map<std::string, Node*> fields;
    fields["v"] = new NumberNode(7);
//    Node* expected_node = LIT_FIL(TYPE("x", {TYPE("y", {})}), fields);
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
    Node* node = parser.parse_expression();
    std::map<std::string, Node*> fields;
//    Node* expected_node = LIT_FIL(TYPE("x", {TYPE("y", {})}), fields);
//    COMPLETE_TEST;
//    EXPECT_EQ(*node, *expected_node);

}

TEST(parser_test, plus_parenthesized_expression) {
    std::string text = "(1+a)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a"));
    COMPLETE_TEST;

}

TEST(parser_test, more_complex_expression) {
    std::string text = "(1+a)+b";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new BinopNode(OpType::ADD, new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a")),
                                        new IdNode("b"));
    COMPLETE_TEST;

}

TEST(parser_test, more_complex_expression_2) {
    std::string text = "b*(1+a)";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new BinopNode(OpType::MUL, new IdNode("b"),
                                        new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a")));
    COMPLETE_TEST;

}

TEST(parser_test, plus_expression) {
    std::string text = "1+a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a"));
    COMPLETE_TEST;

}

TEST(parser_test, minus_expression) {
    std::string text = "a-1";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = new BinopNode(OpType::SUB, new IdNode("a"), new NumberNode(1));
    COMPLETE_TEST;

}

TEST(parser_test, plus_or_minus_with_multiple_terms_expression) {
    std::string text = "a+b-c";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = new BinopNode(OpType::SUB,
                                        new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")),
                                        new IdNode("c"));
    COMPLETE_TEST;

}

TEST(parser_test, mul_or_div_with_multiple_factors_expression) {
    std::string text = "a/b*c";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = new BinopNode(OpType::MUL,
                                        new BinopNode(OpType::DIV, new IdNode("a"), new IdNode("b")),
                                        new IdNode("c"));
    COMPLETE_TEST;

}

TEST(parser_test, times_expression) {
    std::string text = "foo*bar";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_mul_div_or_mod_expression();
    Node* expected_node = new BinopNode(OpType::MUL, new IdNode("foo"), new IdNode("bar"));
    COMPLETE_TEST;

}

TEST(parser_test, boolean_true) {
    std::string text = "true";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new BooleanNode(true);


    COMPLETE_TEST;
}

TEST(parser_test, boolean_false) {
    std::string text = "false";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new BooleanNode(false);
    COMPLETE_TEST;
}

TEST(parser_test, for_loop_1) {
    std::string text = "for(e@l){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ForNode* node = parser.parse_for_loop();
    ForNode* expected_node = new ForNode("e", new IdNode("l"), new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST(parser_test, while_loop_1) {
    std::string text = "while(true){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    WhileNode* node = parser.parse_while_loop();
    WhileNode* expected_node = new WhileNode(new BooleanNode(true), new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST(parser_test, while_loop_common) {
    std::string text = "while(true){}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    WhileNode* node = parser.parse_while_loop();
    WhileNode* expected_node = new WhileNode(new BooleanNode(true), new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST(parser_test, for_loop_2) {
    std::string text = "for(e@[4,5,6]){print(e);}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    ForNode* node = parser.parse_for_loop();
    VectorOfNodes list = {new NumberNode(4), new NumberNode(5), new NumberNode(6)};
    BlockNode* body = new BlockNode({new CallNode(new IdNode("print"), {new IdNode("e")})});
    ForNode* expected_node = new ForNode("e", new ListNode(list), body);
    COMPLETE_TEST;
}

TEST(parser_test, for_loop_3) {
    std::string text = "for(e @[4,5,6]){print(e);}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_top_level_statement();
    VectorOfNodes list = {new NumberNode(4), new NumberNode(5), new NumberNode(6)};
    BlockNode* body = new BlockNode({new CallNode(new IdNode("print"), {new IdNode("e")})});
    Node* expected_node = new ForNode("e", new ListNode(list), body);
    COMPLETE_TEST;
}

TEST(parser_test, div_expression) {
    std::string text = "foo/1";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_mul_div_or_mod_expression();
    Node* expected_node = new BinopNode(OpType::DIV, new IdNode("foo"), new NumberNode(1));
    COMPLETE_TEST;
}

TEST(parser_test, parse_list_empty_no_type_throws_error) {
    std::string text = "[]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    try {
        Node* node = parser.parse_expression();
        FAIL() << "Did not throw an error";
    } catch (...) {

    }
}

TEST(parser_test, parse_xxx) {
    std::string text = "y[2]+x[7] + 43";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    VectorOfNodes list;
    Node* expected_node = new BinopNode(OpType::ADD,
                                        new BinopNode(OpType::ADD,
                                                      new SubscriptNode(new IdNode("y"), {new NumberNode(2)}),
                                                      new SubscriptNode(new IdNode("x"), {new NumberNode(7)})),
                                        new NumberNode(43));
//    
//    
    COMPLETE_TEST;
}

TEST(parser_test, parse_empty_list) {
    std::string text = "[]::Integer";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    VectorOfNodes list;
    Node* expected_node = (new EmptyListNode(T_INT));
//    
//    
    COMPLETE_TEST;
}

TEST(parser_test, parse_list_one_element) {
    std::string text = "[13]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    VectorOfNodes list = {new NumberNode(13)};
    Node* expected_node = new ListNode(list);


    COMPLETE_TEST;
}

TEST(parser_test, parse_none) {
    std::string text = "none";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = (new NoneNode());
    COMPLETE_TEST;
}

TEST(parser_test, parse_list_multiple_elements) {
    std::string text = "[23,17,64]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    VectorOfNodes list = {new NumberNode(23), new NumberNode(17), new NumberNode(64)};
    Node* expected_node = new ListNode(list);


    COMPLETE_TEST;
}

TEST(parser_test, parse_top_level) {
    std::string text = "var x = 9*7;";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_top_level_statement();
    Node* expected_node = new DeclarationNode("x", nullptr,
                                              new BinopNode(OpType::MUL, new NumberNode(9), new NumberNode(7)));
    COMPLETE_TEST;

}

TEST(parser_test, complex_div_expression) {
    std::string text = "foo/(1)+a";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_add_or_sub_expression();
    Node* expected_node = new BinopNode(OpType::ADD, new BinopNode(OpType::DIV, new IdNode("foo"), new NumberNode(1)),
                                        new IdNode("a"));
    COMPLETE_TEST;

}

TEST(parser_test, complex_expression) {
    std::string text = "1-(7)*8-(9*(1-3)/7-8+4)+8";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    Node* expected_node = new BinopNode(OpType::ADD,
                                        new BinopNode(OpType::SUB, new BinopNode(OpType::SUB, new NumberNode(1),
                                                                                 new BinopNode(OpType::MUL,
                                                                                               new NumberNode(7),
                                                                                               new NumberNode(8))),
                                                      new BinopNode(OpType::ADD, new BinopNode(OpType::SUB,
                                                                                               new BinopNode(
                                                                                                       OpType::DIV,
                                                                                                       new BinopNode(
                                                                                                               OpType::MUL,
                                                                                                               new NumberNode(
                                                                                                                       9),
                                                                                                               new BinopNode(
                                                                                                                       OpType::SUB,
                                                                                                                       new NumberNode(
                                                                                                                               1),
                                                                                                                       new NumberNode(
                                                                                                                               3))),
                                                                                                       new NumberNode(
                                                                                                               7)),
                                                                                               new NumberNode(8)),
                                                                    new NumberNode(4))), new NumberNode(8));
    COMPLETE_TEST;

}


TEST(parser_test, complex_chain) {
    std::string text = "a[1][b].c(d[5][0].e).f.g[h][2]()[3][5].i";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    Node* node = parser.parse_expression();
    // a[1][b].c(d[5][0].e
    auto call = new CallNode(new MemberNode(
            new SubscriptNode(new SubscriptNode(new IdNode("a"), {new NumberNode(1)}), {new IdNode("b")}), "c"),
                             {new MemberNode(new SubscriptNode(new SubscriptNode(new IdNode("d"), {new NumberNode(5)}),
                                                               {new NumberNode(0)}), "e")
                             });
    Node* expected_node = new MemberNode(
            new SubscriptNode(new SubscriptNode(new CallNode(new SubscriptNode(
                    new SubscriptNode(new MemberNode(new MemberNode(call, "f"), "g"), {new IdNode("h")}),
                    {new NumberNode(2)}), {}), {new NumberNode(3)}),
                              {new NumberNode(5)}), "i");
    EXPECT_EQ(*node, *expected_node);

}


//TEST(parser_test, super_expression) {
//    std::string text = "1-(a*c()[0]+7/d.a.x(7))*v*c/a+v.x.y[0][1][a+c*7](4,1,b+c)";
//    Scanner scanner(text);
//    std::vector<Token> tokens = scanner.scan_all();
//    Parser parser(tokens);
//    Node* node = parser.parse_expression();
//    Node* expected_node = new BinopNode(OpType::ADD,
//                              new BinopNode(OpType::SUB, new NumberNode(1), new BinopNode(OpType::DIV, new BinopNode(OpType::MUL,
//                                                                            new BinopNode(OpType::MUL,
//                                                                                new BinopNode(OpType::ADD,
//                                                                                    new BinopNode(OpType::MUL,
//                                                                                        new IdNode("a"),
//                                                                                        new SubscriptNode(new CallNode(
//                                                                                                new IdNode("c"),
//                                                                                                {}),
//                                                                                            new NumberNode(
//                                                                                                    0))),
//                                                                                    new BinopNode(OpType::DIV,
//                                                                                        new NumberNode(7),
//                                                                                        new CallNode(MEM(
//                                                                                                MEM(
//                                                                                                        new IdNode("d"),
//                                                                                                        "a"),
//                                                                                                "x"),
//                                                                                             {new NumberNode(7)}))),
//                                                                                new IdNode("v")),
//                                                                            new IdNode("c")),
//                                                           new IdNode("a"))), new CallNode(
//            new SubscriptNode(new SubscriptNode(new SubscriptNode(MEM(MEM(new IdNode("v"), "x"), "y"), new NumberNode(0)), new NumberNode(1)),
//                new BinopNode(OpType::ADD, new IdNode("a"), new BinopNode(OpType::MUL, new IdNode("c"), new NumberNode(7)))),
//            VectorOfNodes({new NumberNode(4), new NumberNode(1), new BinopNode(OpType::ADD, new IdNode("b"), new IdNode("c"))})));
//    std::cerr << *node << std::endl;
//
//    COMPLETE_TEST;
//}
