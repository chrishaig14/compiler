//
// Created by chris on 14/6/20.
//

#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <parser/UnexpectedToken.h>
#include <macros.h>


#define BODY_NODE new BlockNode({new AssignmentNode(new IdNode("x"), new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b"))),new AssignmentNode(new IdNode("x"), new IdNode("y"))})
#define FUN_FOO_NODE FUN("foo", { "x" }, VectorOfTypes({ COMPLEX_TYPE }), TYPE("List", {new T_INT}), BODY_NODE)
#define FUN_FOO_STRING "fun foo(x:List[List[Integer]])->List[Integer]{x=a+b; x = y;}"
#define COMPLEX_TYPE new T_LIST(new T_LIST(new T_INT))

#define EXPECT_NOT_EQUAL EXPECT_FALSE(node->equal(expected_node)); delete node; delete expected_node;

class parser_test : public ::testing::Test {
protected:
    Scanner* scanner;
    Node* node;
    Node* expected_node;
    TypeNode* t_node;
    TypeNode* t_expected_node;

    void SetUp() override {
        scanner = nullptr;
        node = nullptr;
        expected_node = nullptr;
        t_node = nullptr;
        t_expected_node = nullptr;
    }

    void SetUp(const std::string& text) {
        scanner = new Scanner(text);
        node = nullptr;
        expected_node = nullptr;
    }

    void TearDown() override {
        if (scanner != nullptr) {
            delete scanner;
        }
        if (node != nullptr) {
            delete node;
        }
        if (expected_node != nullptr) {
            delete expected_node;
        }
        if (t_node != nullptr) {
            delete t_node;
        }
        if (t_expected_node != nullptr) {
            delete t_expected_node;
        }
    }
};

TEST_F(parser_test, a_plus_b) {
    std::string text = "a + b";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_add_or_sub_expression();
    expected_node = (
            new BinopNode(OpType::ADD, (new IdNode("a")), (new IdNode("b"))));
    EXPECT_EQ(node->ntype, expected_node->ntype);
    EXPECT_EQ(node->binop(), expected_node->binop());
}

TEST_F(parser_test, ternary) {
    std::string text = "a?7:6";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_ternary();
    expected_node = new TernaryNode(new IdNode("a"), new NumberNode(7), new NumberNode(6));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, a_or_b) {
    std::string text = "a||b";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_or_expression();
    expected_node = new BinopNode(OpType::OR, new IdNode("a"), new IdNode("b"));
    EXPECT_EQ(node->ntype, expected_node->ntype);
    EXPECT_EQ(node->binop(), expected_node->binop());
}

TEST_F(parser_test, complex_expression_1) {
    std::string text = "a||s-c==7?8-9?7:4:10";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new TernaryNode(
            new BinopNode(
                    OpType::OR, new IdNode("a"),
                    new BinopNode(
                            OpType::EQ, new BinopNode(OpType::SUB, new IdNode("s"), new IdNode("c")),
                            new NumberNode(7))),
            new TernaryNode(
                    new BinopNode(OpType::SUB, new NumberNode(8), new NumberNode(9)), new NumberNode(7),
                    new NumberNode(4)),
            new NumberNode(10));
    EXPECT_EQ(node->ntype, expected_node->ntype);
    EXPECT_EQ(node->ternary(), expected_node->ternary());
}

TEST_F(parser_test, a_eq_b) {
    std::string text = "a==b";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new BinopNode(OpType::EQ, new IdNode("a"), new IdNode("b"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, id) {
    std::string text = "foo";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_add_or_sub_expression();
    expected_node = new IdNode("foo");
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, assign_x_y) {
    std::string text = "x = y";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_assignment_or_expression();
    expected_node = new AssignmentNode(new IdNode("x"), new IdNode("y"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, assign_x_a_plus_b) {
    std::string text = "x = a+b";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_assignment_or_expression();
    expected_node = new AssignmentNode(
            new IdNode("x"),
            new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, declare_x_no_type_no_exp_error) {
    std::string text = "var x";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    try {
        node = parser.parse_variable_declaration();
        FAIL() << "Didn't throw  unexpected token error";
    } catch (...) {

    }
}

TEST_F(parser_test, declare_x_a_plus_b) {
    std::string text = "var x = a + b";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_variable_declaration();
    expected_node = new DeclarationNode(
            "x", nullptr,
            new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, empty_block) {
    std::string text = "{}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_possibly_empty_block();
    BlockNode* block_node = &node->block();
    EXPECT_EQ(block_node->nodes.size(), 0);
}

TEST_F(parser_test, non_empty_block) {
    std::string text = "{x = a + b; x = y;}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_possibly_empty_block();
    expected_node = new BlockNode(
            {new AssignmentNode(
                    new IdNode("x"),
                    new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b"))),
             new AssignmentNode(new IdNode("x"), new IdNode("y"))}
    );
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, if_x_empty_then) {
    std::string text = "if(x){}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_if();
    expected_node = new IfNode(new IdNode("x"), new BlockNode({}), {}, {});
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, if_with_else) {
    std::string text = "if(x){}else{}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_if();
    expected_node = new IfNode(new IdNode("x"), new BlockNode({}), {}, new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, if_x_non_empty_then) {
//    std::string text = "if(x){x=a+b; x = y;}";
//    SetUp(text);
//    std::vector<Token> tokens = scanner->scan_all();
//    Parser parser(tokens);
//    node = parser.parse_if();
//    auto st_1 = new AssignmentNode(new IdNode("x"), new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
//    auto st_2 = new AssignmentNode(new IdNode("x"), new IdNode("y"));
////    expected_node = IF(new IdNode("x"), new BlockNode({st_1, st_2}));
////    EXPECT_EQ(*node,*expected_node);

}

TEST_F(parser_test, simple_type) {
    std::string text = "String";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    t_node = parser.parse_type_node();
    t_expected_node = new T_STRING;
    EXPECT_EQ(*t_node, *t_expected_node);
}

TEST_F(parser_test, template_type) {
    std::string text = "String[Integer]";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    t_node = parser.parse_type_node();
    t_expected_node = TYPE("String", VectorOfTypes{TYPE("Integer", {})});
//    EXPECT_EQ(*node,*expected_node);
    EXPECT_EQ(*t_node, *t_expected_node);
}

TEST_F(parser_test, fun_empty) {
    std::string text = "fun() ->String";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    t_node = parser.parse_type_node();
    t_expected_node = FUNCTION_TYPE({}, new T_STRING);
    EXPECT_EQ(*t_node, *t_expected_node);

//    EXPECT_EQ(*node,*expected_node);
}

TEST_F(parser_test, fun_full) {
    std::string text = "fun(List[String],fun(String)->Integer) -> List[Integer]";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    t_node = parser.parse_type_node();
    VectorOfTypes t = {new T_LIST(new T_STRING), FUNCTION_TYPE({ new T_STRING }, new T_INT)};
    t_expected_node = FUNCTION_TYPE(t, new T_LIST(new T_INT));
    EXPECT_EQ(*t_node, *t_expected_node);
}


TEST_F(parser_test, complex_template_type) {
    std::string text = "String[Integer, List[String]]";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    t_node = parser.parse_type_node();
    t_expected_node = TYPE("String", VectorOfTypes({new T_INT, new T_LIST({new T_STRING})}));
    EXPECT_EQ(*t_node, *t_expected_node);
}

TEST_F(parser_test, decl_with_type_and_value) {
    std::string text = "var x:String[Integer, List[String]] = a + b";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_variable_declaration();
    expected_node = new DeclarationNode(
            "x", TYPE("String", VectorOfTypes({new T_INT, new T_LIST({new T_STRING})})),
            new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, function_no_params_empty_body) {
    std::string text = "fun foo()->String{}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_function_definition();
    auto return_type = TYPE("String", {});
    BlockNode* b = new BlockNode({});
    expected_node = new FunctionNode("foo", std::vector<std::string>(), VectorOfTypes(), return_type, b);
//    EXPECT_EQ(*node,*expected_node);
    EXPECT_EQ(*node, *expected_node);
}


TEST_F(parser_test, function_with_params_empty_body) {
    std::string text = "fun foo(x:List[List[Integer]])->Integer{}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_function_definition();
    BlockNode* b = new BlockNode({});
    expected_node = new FunctionNode("foo", {"x"}, {COMPLEX_TYPE}, TYPE("Integer", {}), b);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, function_with_params_and_body) {
    std::string text = "fun foo(x:List[List[Integer]])->String{x=a+b; x = y;}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_function_definition();
    BlockNode* b = new BlockNode(
            {new AssignmentNode(new IdNode("x"), new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b"))),
             new AssignmentNode(new IdNode("x"), new IdNode("y"))}
    );
    expected_node = new FunctionNode("foo", {"x"}, {COMPLEX_TYPE}, TYPE("String", {}), b);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, class_literal_fil) {
//    std::string text = "#Person{name: name, age: 27*32}";
//    SetUp(text);
//    std::vector<Token> tokens = scanner->scan_all();
//    Parser parser(tokens);
//    node = parser.parse_factor();
//    std::map<std::string, Node*> init = {{"name", new IdNode("name")},
//                                         {"age",  new BinopNode(OpType::MUL, new NumberNode(27), new NumberNode(32))}};
////    expected_node = LIT_FIL(TYPE("Person", {}), init);
////    EXPECT_EQ(*node,*expected_node);
}

TEST_F(parser_test, class_literal_empty_ok) {
//    std::string text = "#Person{}";
//    SetUp(text);
//    std::vector<Token> tokens = scanner->scan_all();
//    Parser parser(tokens);
//    node = parser.parse_factor();
//    VectorOfNodes init = {};
//    expected_node = LIT_EXP(TYPE("Person", {}), init);
//    EXPECT_EQ(*node,*expected_node);
}

//TEST_F(parser_test, class_literal_error_2) {
//    std::string text = "#Person{name, age: 32}";
//    SetUp(text);
//    std::vector<Token> tokens = scanner->scan_all();
//    Parser parser(tokens);
//    try {
//        node = parser.parse_factor();
//        FAIL() << "Didn't throw UnexpectedToken";
//    } catch (const UnexpectedToken& e) {
//
//    }
//}

TEST_F(parser_test, function_with_params_return_type_and_body) {
//    std::string text = FUN_FOO_STRING;
//    SetUp(text);
//    std::vector<Token> tokens = scanner->scan_all();
//    Parser parser(tokens);
//    node = parser.parse_function_definition();
////    expected_node = FUN_FOO_NODE;
////    EXPECT_EQ(*node,*expected_node);

}

TEST_F(parser_test, class_literal_fields) {
    std::string text = "#Foo{ y: 27, x: 9}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    std::unordered_map<std::string, Node*> fields;
    fields["y"] = new NumberNode(27);
    fields["x"] = new NumberNode(9);
    expected_node = new ClassLiteralFieldNode(TYPE("Foo", {}), fields);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, class_literal_expression_ok) {
    std::string text = "#Foo{9,27}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    VectorOfNodes fields;
    fields.push_back(new NumberNode(9));
    fields.push_back(new NumberNode(27));
    expected_node = new ClassLiteralExpressionNode(TYPE("Foo", {}), fields);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_member) {
    std::string text = "a.b";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new MemberNode(new IdNode("a"), "b");
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_id) {
    std::string text = "a";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new IdNode("a");
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_call) {
    std::string text = "a()";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new CallNode(new IdNode("a"), {});
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_subscript) {
    std::string text = "a[1]";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new SubscriptNode(new IdNode("a"), {new NumberNode(1)});
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, literal_number_expression) {
    std::string text = "7";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new NumberNode(7);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_parenthesized_expression) {
    std::string text = "(1)";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new NumberNode(1);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, test_now_1) {
    std::string text = "x";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new IdNode("x");
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, test_now_2) {
    std::string text = "x[7]";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new SubscriptNode(new IdNode("x"), {new NumberNode(7)});
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, test_now_3_should_fail) {
    std::string text = "x[7]{}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new SubscriptNode(new IdNode("x"), {new NumberNode(7)});
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, test_now_3) {
//    std::string text = "#x[y]{v:7}";
//    SetUp(text);
//    std::vector<Token> tokens = scanner->scan_all();
//    Parser parser(tokens);
//    node = parser.parse_expression();
//    std::map<std::string, Node*> fields;
//    fields["v"] = new NumberNode(7);
////    expected_node = LIT_FIL(TYPE("x", {TYPE("y", {})}), fields);
////    EXPECT_EQ(*node,*expected_node);
}

TEST_F(parser_test, test_now_4) {
    std::string text = "#x[y]";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    try {
        parser.parse_expression();
        FAIL() << "Expected exception!";
    } catch (const UnexpectedToken& e) {

    }
}

TEST_F(parser_test, test_now_8) {
//    std::string text = "#x[y]{}";
//    SetUp(text);
//    std::vector<Token> tokens = scanner->scan_all();
//    Parser parser(tokens);
//    node = parser.parse_expression();
//    std::map<std::string, Node*> fields;
////    expected_node = LIT_FIL(TYPE("x", {TYPE("y", {})}), fields);
////    EXPECT_EQ(*node,*expected_node);
////    EXPECT_EQ(*node, *expected_node);

}

TEST_F(parser_test, plus_parenthesized_expression) {
    std::string text = "(1+a)";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, more_complex_expression) {
    std::string text = "(1+a)+b";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new BinopNode(
            OpType::ADD, new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a")),
            new IdNode("b"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, more_complex_expression_2) {
    std::string text = "b*(1+a)";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new BinopNode(
            OpType::MUL, new IdNode("b"),
            new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a")));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, plus_expression) {
    std::string text = "1+a";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_add_or_sub_expression();
    expected_node = new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a"));
    EXPECT_EQ(*node, *expected_node);

}

TEST_F(parser_test, minus_expression) {
    std::string text = "a-1";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_add_or_sub_expression();
    expected_node = new BinopNode(OpType::SUB, new IdNode("a"), new NumberNode(1));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, plus_or_minus_with_multiple_terms_expression) {
    std::string text = "a+b-c";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_add_or_sub_expression();
    expected_node = new BinopNode(
            OpType::SUB,
            new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")),
            new IdNode("c"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, mul_or_div_with_multiple_factors_expression) {
    std::string text = "a/b*c";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_add_or_sub_expression();
    expected_node = new BinopNode(
            OpType::MUL,
            new BinopNode(OpType::DIV, new IdNode("a"), new IdNode("b")),
            new IdNode("c"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, times_expression) {
    std::string text = "foo*bar";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_mul_div_or_mod_expression();
    expected_node = new BinopNode(OpType::MUL, new IdNode("foo"), new IdNode("bar"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, boolean_true) {
    std::string text = "true";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new BooleanNode(true);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, boolean_false) {
    std::string text = "false";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new BooleanNode(false);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, for_loop_1) {
    std::string text = "for(e@l){}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_for_loop();
    expected_node = new ForNode("e", new IdNode("l"), new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, while_loop_1) {
    std::string text = "while(true){}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_while_loop();
    expected_node = new WhileNode(new BooleanNode(true), new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, while_loop_common) {
    std::string text = "while(true){}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_while_loop();
    expected_node = new WhileNode(new BooleanNode(true), new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, for_loop_2) {
    std::string text = "for(e@[4,5,6]){print(e);}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_for_loop();
    VectorOfNodes list = {new NumberNode(4), new NumberNode(5), new NumberNode(6)};
    BlockNode* body = new BlockNode({new CallNode(new IdNode("print"), {new IdNode("e")})});
    expected_node = new ForNode("e", new ListNode(list), body);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, for_loop_3) {
    std::string text = "for(e @[4,5,6]){print(e);}";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_top_level_statement();
    VectorOfNodes list = {new NumberNode(4), new NumberNode(5), new NumberNode(6)};
    BlockNode* body = new BlockNode({new CallNode(new IdNode("print"), {new IdNode("e")})});
    expected_node = new ForNode("e", new ListNode(list), body);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, div_expression) {
    std::string text = "foo/1";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_mul_div_or_mod_expression();
    expected_node = new BinopNode(OpType::DIV, new IdNode("foo"), new NumberNode(1));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_list_empty_no_type_throws_error) {
    std::string text = "[]";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    try {
        node = parser.parse_expression();
        FAIL() << "Did not throw an error";
    } catch (...) {

    }
}

TEST_F(parser_test, parse_xxx) {
    std::string text = "y[2]+x[7] + 43";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    VectorOfNodes list;
    expected_node = new BinopNode(
            OpType::ADD,
            new BinopNode(
                    OpType::ADD,
                    new SubscriptNode(new IdNode("y"), {new NumberNode(2)}),
                    new SubscriptNode(new IdNode("x"), {new NumberNode(7)})),
            new NumberNode(43));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_empty_list) {
    std::string text = "[]::Integer";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    VectorOfNodes list;
    expected_node = (new EmptyListNode(new T_INT));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_list_one_element) {
    std::string text = "[13]";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    VectorOfNodes list = {new NumberNode(13)};
    expected_node = new ListNode(list);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_none) {
    std::string text = "none";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = (new NoneNode());
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_list_multiple_elements) {
    std::string text = "[23,17,64]";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    VectorOfNodes list = {new NumberNode(23), new NumberNode(17), new NumberNode(64)};
    expected_node = new ListNode(list);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_top_level) {
    std::string text = "var x = 9*7;";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_top_level_statement();
    expected_node = new DeclarationNode(
            "x", nullptr,
            new BinopNode(OpType::MUL, new NumberNode(9), new NumberNode(7)));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, complex_div_expression) {
    std::string text = "foo/(1)+a";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_add_or_sub_expression();
    expected_node = new BinopNode(
            OpType::ADD, new BinopNode(OpType::DIV, new IdNode("foo"), new NumberNode(1)),
            new IdNode("a"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, complex_expression) {
    std::string text = "1-(7)*8-(9*(1-3)/7-8+4)+8";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    expected_node = new BinopNode(
            OpType::ADD,
            new BinopNode(
                    OpType::SUB, new BinopNode(
                            OpType::SUB, new NumberNode(1),
                            new BinopNode(
                                    OpType::MUL,
                                    new NumberNode(7),
                                    new NumberNode(8))),
                    new BinopNode(
                            OpType::ADD, new BinopNode(
                                    OpType::SUB,
                                    new BinopNode(
                                            OpType::DIV,
                                            new BinopNode(
                                                    OpType::MUL,
                                                    new NumberNode(
                                                            9
                                                    ),
                                                    new BinopNode(
                                                            OpType::SUB,
                                                            new NumberNode(
                                                                    1
                                                            ),
                                                            new NumberNode(
                                                                    3
                                                            ))),
                                            new NumberNode(
                                                    7
                                            )),
                                    new NumberNode(8)),
                            new NumberNode(4))), new NumberNode(8));
    EXPECT_EQ(*node, *expected_node);

}


TEST_F(parser_test, complex_chain) {
    std::string text = "a[1][b].c(d[5][0].e).f.g[h][2]()[3][5].i";
    SetUp(text);
    std::vector<Token> tokens = scanner->scan_all();
    Parser parser(tokens);
    node = parser.parse_expression();
    // a[1][b].c(d[5][0].e
    auto call = new CallNode(
            new MemberNode(
                    new SubscriptNode(new SubscriptNode(new IdNode("a"), {new NumberNode(1)}), {new IdNode("b")}), "c"
            ),
            {new MemberNode(
                    new SubscriptNode(
                            new SubscriptNode(new IdNode("d"), {new NumberNode(5)}),
                            {new NumberNode(0)}
                    ), "e"
            )
            }
    );
    expected_node = new MemberNode(
            new SubscriptNode(
                    new SubscriptNode(
                            new CallNode(
                                    new SubscriptNode(
                                            new SubscriptNode(
                                                    new MemberNode(new MemberNode(call, "f"), "g"),
                                                    {new IdNode("h")}
                                            ),
                                            {new NumberNode(2)}
                                    ), {}
                            ), {new NumberNode(3)}
                    ),
                    {new NumberNode(5)}
            ), "i"
    );
    EXPECT_EQ(*node, *expected_node);
}


//TEST_F(parser_test, super_expression) {
//    std::string text = "1-(a*c()[0]+7/d.a.x(7))*v*c/a+v.x.y[0][1][a+c*7](4,1,b+c)";
//    SetUp(text);
//    std::vector<Token> tokens = scanner->scan_all();
//    Parser parser(tokens);
//    node = parser.parse_expression();
//    expected_node = new BinopNode(OpType::ADD,
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
//    EXPECT_EQ(*node,*expected_node);
//}
