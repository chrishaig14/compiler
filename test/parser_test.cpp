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

BlockNode* full_parse(const std::string& __file__, CodeLines* code_lines) {
    BlockNode* tree;
    Scanner scanner(__file__);
    std::vector<Token> tokens = scanner.scan_all();
    // Token token;
    // for (auto token: tokens) {
    //     std::cout << token.to_string() << std::endl;
    // }
    Parser parser(__file__, scanner.code_lines, tokens);
    *code_lines = scanner.code_lines;
    try {
        tree = parser.parse_program();
    } catch (const UnexpectedToken& ut) {
        std::cerr << ut.what() << std::endl;
        exit(1);
    }
    return tree;
}

class parser_test : public ::testing::Test {
protected:
    Scanner* scanner;
    Node* node;
    Node* expected_node;
    TypeNode* t_node;
    TypeNode* t_expected_node;
    std::string file;
    Parser* parser;

    void SetUp() override {
        scanner = nullptr;
        node = nullptr;
        expected_node = nullptr;
        t_node = nullptr;
        t_expected_node = nullptr;
    }

    void SetUp(const std::string& text) {
        file = tmpnam(nullptr);
        std::ofstream f(file);
        f << text;
        f.close();
        scanner = new Scanner(file);
        node = nullptr;
        expected_node = nullptr;
        std::vector<Token> tokens = scanner->scan_all();
        parser = new Parser(file, scanner->code_lines, tokens);
    }

    void TearDown() override {
        unlink(file.c_str());
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

void ASSERT_PARSE_ERROR(Parser* parser, const std::string& msg) {
    try {
        parser->parse_program();
        // EXPECT_THROW()
        FAIL() << "DID NOT THROW";
    } catch (const std::runtime_error& e) {
        std::string s = e.what();
        if (s.find(msg) == -1) {
            FAIL() << "ERROR DID NOT MATCH, GOT " << s;
        }
    }
}

TEST_F(parser_test, error_1) {
    std::string text = "fun main()->Integer{x=}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_2) {
    std::string text = "fun main()->Integer{x=43+}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_3) {
    std::string text = "fun main()->Integer{x=43+[}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_4) {
    std::string text = "fun main()->{}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected type");
}

TEST_F(parser_test, error_5) {
    std::string text = "fun main()->List[{}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected type");
}

TEST_F(parser_test, error_6) {
    std::string text = "fun main()->Dict[Integer,{}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected type");
}

TEST_F(parser_test, error_7) {
    std::string text = "fun main()->List[Integer{}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_8) {
    std::string text = "fun (";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_9) {
    std::string text = "fun main+";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_10) {
    std::string text = "fun main(123";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_11) {
    std::string text = "fun main(x 12";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_12) {
    std::string text = "fun main(x: 123";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected type");
}

TEST_F(parser_test, error_13) {
    std::string text = "fun main(x: Integer,)";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_14) {
    std::string text = "fun main(x: Integer)12";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_15) {
    std::string text = "fun main(x: Integer)->{";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected type");
}

TEST_F(parser_test, error_16) {
    std::string text = "fun main(x: Integer)->String(";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_17) {
    std::string text = "fun main(x: Integer)->String{break}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "break out of loop");
}

TEST_F(parser_test, error_18) {
    std::string text = "fun main(x: Integer)->String{continue}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "continue out of loop");
}

TEST_F(parser_test, error_19) {
    std::string text = "fun main(x: Integer)->String{2+5}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_20) {
    std::string text = "fun main(x: Integer)->String{var 23}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_21) {
    std::string text = "fun main(x: Integer)->String{var x + }";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_22) {
    std::string text = "fun main(x: Integer)->String{var x : 23 }";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected type");
}

TEST_F(parser_test, error_23) {
    std::string text = "fun main(x: Integer)->String{var x : Integer .,., }";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_24) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = ,,}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_25) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = 23,}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_26) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = 23(}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_27) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo)}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_28) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = s[]}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_29) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = s[24,]}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_30) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo(.,}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_31) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo(23,}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_32) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo(23,123]}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_33) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo(23,123);if ,.}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_34) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo(23,123);if 23+{}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_35) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo(23,123);if 23,{}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_36) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo(23,123);for 23}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_37) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo(23,123);for wqe 2}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_38) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo(23,123);for wqe @ ,..}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_39) {
    std::string text = "fun main(x: Integer)->String{var x : Integer = foo(23,123);for wqe @ l]}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_40) {
    std::string text = "fun main(x: Integer)->String{var x = [.}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, error_41) {
    std::string text = "fun main(x: Integer)->String{var x = [23=}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "Unexpected token");
}

TEST_F(parser_test, error_42) {
    std::string text = "fun main(x: Integer)->String{var x = [23,}";
    SetUp(text);
    ASSERT_PARSE_ERROR(parser, "expected expression");
}

TEST_F(parser_test, a_plus_b) {
    std::string text = "a + b";
    SetUp(text);
    node = parser->parse_add_or_sub_expression();
    expected_node = (
            new BinopNode(OpType::ADD, (new IdNode("a")), (new IdNode("b"))));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, ternary) {
    std::string text = "a?7:6";
    SetUp(text);
    node = parser->parse_ternary();
    expected_node = new TernaryNode(new IdNode("a"), new NumberNode(7), new NumberNode(6));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, a_or_b) {
    std::string text = "a||b";
    SetUp(text);
    node = parser->parse_or_expression();
    expected_node = new BoolOpNode(BoolOp::OR, new IdNode("a"), new IdNode("b"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, complex_expression_1) {
    std::string text = "a||s-c==7?8-9?7:4:10";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new TernaryNode(
            new BoolOpNode(
                    BoolOp::OR, new IdNode("a"),
                    new BoolOpNode(
                            BoolOp::EQ, new BinopNode(OpType::SUB, new IdNode("s"), new IdNode("c")),
                            new NumberNode(7))),
            new TernaryNode(
                    new BinopNode(OpType::SUB, new NumberNode(8), new NumberNode(9)), new NumberNode(7),
                    new NumberNode(4)),
            new NumberNode(10));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, a_eq_b) {
    std::string text = "a==b";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new BoolOpNode(BoolOp::EQ, new IdNode("a"), new IdNode("b"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, id) {
    std::string text = "foo";
    SetUp(text);
    node = parser->parse_add_or_sub_expression();
    expected_node = new IdNode("foo");
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, flot) {
    std::string text = "43.567";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new FloatNode(43.567f);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, float_with_member) {
    std::string text = "0.567.foo";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new MemberNode(new FloatNode(0.567f), "foo");
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, float_without_zero_error) {
    std::string text = ".567";
    SetUp(text);
    try {
        node = parser->parse_expression();
        FAIL();
    } catch (...) {

    }
}


TEST_F(parser_test, assign_x_y) {
    std::string text = "x = y";
    SetUp(text);
    node = parser->parse_assignment_or_expression();
    expected_node = new AssignmentNode(new IdNode("x"), new IdNode("y"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, assign_x_a_plus_b) {
    std::string text = "x = a+b";
    SetUp(text);
    node = parser->parse_assignment_or_expression();
    expected_node = new AssignmentNode(
            new IdNode("x"),
            new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, declare_x_no_type_no_exp_error) {
    std::string text = "var x";
    SetUp(text);
    try {
        node = parser->parse_variable_declaration();
        FAIL() << "Didn't throw  unexpected token error";
    } catch (...) {

    }
}

TEST_F(parser_test, declare_x_a_plus_b) {
    std::string text = "var x = a + b";
    SetUp(text);
    node = parser->parse_variable_declaration();
    expected_node = new DeclarationNode(
            "x", nullptr,
            new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, empty_block) {
    std::string text = "{}";
    SetUp(text);
    node = parser->parse_possibly_empty_block();
    BlockNode* block_node = &node->block();
    EXPECT_EQ(block_node->nodes.size(), 0);
}

TEST_F(parser_test, non_empty_block) {
    std::string text = "{x = a + b; x = y;}";
    SetUp(text);
    node = parser->parse_possibly_empty_block();
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
    node = parser->parse_if();
    expected_node = new IfNode(new IdNode("x"), new BlockNode({}), {}, {});
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, if_with_else) {
    std::string text = "if(x){}else{}";
    SetUp(text);
    node = parser->parse_if();
    expected_node = new IfNode(new IdNode("x"), new BlockNode({}), {}, new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_type) {
    std::string text = "String";
    SetUp(text);
    t_node = parser->parse_type_node();
    t_expected_node = new T_STRING;
    EXPECT_EQ(*t_node, *t_expected_node);
}

TEST_F(parser_test, template_type) {
    std::string text = "String[Integer]";
    SetUp(text);
    t_node = parser->parse_type_node();
    t_expected_node = TYPE("String", VectorOfTypes{TYPE("Integer", {})});
//    EXPECT_EQ(*node,*expected_node);
    EXPECT_EQ(*t_node, *t_expected_node);
}

TEST_F(parser_test, fun_empty) {
    std::string text = "fun() ->String";
    SetUp(text);
    t_node = parser->parse_type_node();
    t_expected_node = FUNCTION_TYPE({}, new T_STRING);
    EXPECT_EQ(*t_node, *t_expected_node);

//    EXPECT_EQ(*node,*expected_node);
}

TEST_F(parser_test, fun_full) {
    std::string text = "fun(List[String],fun(String)->Integer) -> List[Integer]";
    SetUp(text);
    t_node = parser->parse_type_node();
    VectorOfTypes t = {new T_LIST(new T_STRING), FUNCTION_TYPE({ new T_STRING }, new T_INT)};
    t_expected_node = FUNCTION_TYPE(t, new T_LIST(new T_INT));
    EXPECT_EQ(*t_node, *t_expected_node);
}


TEST_F(parser_test, complex_template_type) {
    std::string text = "String[Integer, List[String]]";
    SetUp(text);
    t_node = parser->parse_type_node();
    t_expected_node = TYPE("String", VectorOfTypes({new T_INT, new T_LIST({new T_STRING})}));
    EXPECT_EQ(*t_node, *t_expected_node);
}

TEST_F(parser_test, decl_with_type_and_value) {
    std::string text = "var x:String[Integer, List[String]] = a + b";
    SetUp(text);
    node = parser->parse_variable_declaration();
    expected_node = new DeclarationNode(
            "x", TYPE("String", VectorOfTypes({new T_INT, new T_LIST({new T_STRING})})),
            new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, function_no_params_empty_body) {
    std::string text = "fun foo()->String{}";
    SetUp(text);
    node = parser->parse_function_definition();
    auto return_type = TYPE("String", {});
    BlockNode* b = new BlockNode({});
    expected_node = new FunctionNode("foo", VectorOfStrings(), VectorOfTypes(), return_type, b);
//    EXPECT_EQ(*node,*expected_node);
    EXPECT_EQ(*node, *expected_node);
}


TEST_F(parser_test, function_with_params_empty_body) {
    std::string text = "fun foo(x:List[List[Integer]])->Integer{}";
    SetUp(text);
    node = parser->parse_function_definition();
    BlockNode* b = new BlockNode({});
    expected_node = new FunctionNode("foo", {"x"}, {COMPLEX_TYPE}, TYPE("Integer", {}), b);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, function_with_params_and_body) {
    std::string text = "fun foo(x:List[List[Integer]])->String{x=a+b; x = y;}";
    SetUp(text);
    node = parser->parse_function_definition();
    BlockNode* b = new BlockNode(
            {new AssignmentNode(new IdNode("x"), new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b"))),
             new AssignmentNode(new IdNode("x"), new IdNode("y"))}
    );
    expected_node = new FunctionNode("foo", {"x"}, {COMPLEX_TYPE}, TYPE("String", {}), b);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, tuple_literal_one_element_error) {
    std::string text = "#(23)";
    SetUp(text);
    try {
        node = parser->parse_expression();
    } catch (...) {

    }
}

TEST_F(parser_test, tuple_literal_multi_elements_ok) {
    std::string text = "#(23, \"Hello\", 43 + 9)";
    SetUp(text);
    node = parser->parse_expression();
    VectorOfNodes values = {NUM(23), STR("Hello"), BIN(OpType::ADD, NUM(43), NUM(9))};
    expected_node = new TupleNode(values);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, tuple_literal_empty_error) {
    std::string text = "#()";
    SetUp(text);
    try {
        node = parser->parse_expression();
        FAIL();
    } catch (...) {

    }
}

TEST_F(parser_test, class_literal_fields) {
    std::string text = "#Foo{ y: 27, x: 9}";
    SetUp(text);
    node = parser->parse_expression();
    std::unordered_map<std::string, Node*> fields;
    fields["y"] = new NumberNode(27);
    fields["x"] = new NumberNode(9);
    expected_node = new ClassLiteralFieldNode(TYPE("Foo", {}), fields);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, partial_function) {
    std::string text = "$sum(4, *, \"Hello\")";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new PartialApplication(
            new IdNode("sum"),
            VectorOfNodes({new NumberNode(4), nullptr, new StringNode("Hello")}));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, class_literal_expression_ok) {
    std::string text = "#Foo{9,27}";
    SetUp(text);
    node = parser->parse_expression();
    VectorOfNodes fields;
    fields.push_back(new NumberNode(9));
    fields.push_back(new NumberNode(27));
    expected_node = new ClassLiteralExpressionNode(TYPE("Foo", {}), fields);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_member_str) {
    std::string text = "a.b";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new MemberNode(new IdNode("a"), "b");
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_member_num) {
    std::string text = "a.7";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new MemberNode(new IdNode("a"), 7);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_id) {
    std::string text = "a";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new IdNode("a");
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_call) {
    std::string text = "a()";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new CallNode(new IdNode("a"), {});
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_subscript) {
    std::string text = "a[1]";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new SubscriptNode(new IdNode("a"), {new NumberNode(1)});
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, literal_number_expression) {
    std::string text = "7";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new NumberNode(7);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, simple_parenthesized_expression) {
    std::string text = "(1)";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new NumberNode(1);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, test_now_1) {
    std::string text = "x";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new IdNode("x");
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, test_now_2) {
    std::string text = "x[7]";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new SubscriptNode(new IdNode("x"), {new NumberNode(7)});
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, test_now_3_should_fail) {
    std::string text = "x[7]{}";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new SubscriptNode(new IdNode("x"), {new NumberNode(7)});
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, test_now_4) {
    std::string text = "#x[y]";
    SetUp(text);
    try {
        parser->parse_expression();
        FAIL() << "Expected exception!";
    } catch (const UnexpectedToken& e) {

    }
}

TEST_F(parser_test, plus_parenthesized_expression) {
    std::string text = "(1+a)";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, more_complex_expression) {
    std::string text = "(1+a)+b";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new BinopNode(
            OpType::ADD, new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a")),
            new IdNode("b"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, more_complex_expression_2) {
    std::string text = "b*(1+a)";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new BinopNode(
            OpType::MUL, new IdNode("b"),
            new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a")));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, plus_expression) {
    std::string text = "1+a";
    SetUp(text);
    node = parser->parse_add_or_sub_expression();
    expected_node = new BinopNode(OpType::ADD, new NumberNode(1), new IdNode("a"));
    EXPECT_EQ(*node, *expected_node);

}

TEST_F(parser_test, minus_expression) {
    std::string text = "a-1";
    SetUp(text);
    node = parser->parse_add_or_sub_expression();
    expected_node = new BinopNode(OpType::SUB, new IdNode("a"), new NumberNode(1));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, plus_or_minus_with_multiple_terms_expression) {
    std::string text = "a+b-c";
    SetUp(text);
    node = parser->parse_add_or_sub_expression();
    expected_node = new BinopNode(
            OpType::SUB,
            new BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")),
            new IdNode("c"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, mul_or_div_with_multiple_factors_expression) {
    std::string text = "a/b*c";
    SetUp(text);
    node = parser->parse_add_or_sub_expression();
    expected_node = new BinopNode(
            OpType::MUL,
            new BinopNode(OpType::DIV, new IdNode("a"), new IdNode("b")),
            new IdNode("c"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, times_expression) {
    std::string text = "foo*bar";
    SetUp(text);
    node = parser->parse_mul_div_or_mod_expression();
    expected_node = new BinopNode(OpType::MUL, new IdNode("foo"), new IdNode("bar"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, boolean_true) {
    std::string text = "true";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new BooleanNode(true, TextPosition());
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, boolean_false) {
    std::string text = "false";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = new BooleanNode(false, TextPosition());
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, for_loop_1) {
    std::string text = "for(e@l){}";
    SetUp(text);
    node = parser->parse_for_loop();
    expected_node = new ForNode("e", new IdNode("l"), new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, while_loop_1) {
    std::string text = "while(true){}";
    SetUp(text);
    node = parser->parse_while_loop();
    expected_node = new WhileNode(new BooleanNode(true, TextPosition()), new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, while_loop_common) {
    std::string text = "while(true){}";
    SetUp(text);
    node = parser->parse_while_loop();
    expected_node = new WhileNode(new BooleanNode(true, TextPosition()), new BlockNode({}));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, for_loop_2) {
    std::string text = "for(e@[4,5,6]){print(e);}";
    SetUp(text);
    node = parser->parse_for_loop();
    VectorOfNodes list = {new NumberNode(4), new NumberNode(5), new NumberNode(6)};
    BlockNode* body = new BlockNode({new CallNode(new IdNode("print"), {new IdNode("e")})});
    expected_node = new ForNode("e", new ListNode(list), body);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, for_loop_3) {
    std::string text = "for(e @[4,5,6]){print(e);}";
    SetUp(text);
    node = parser->parse_top_level_statement();
    VectorOfNodes list = {new NumberNode(4), new NumberNode(5), new NumberNode(6)};
    BlockNode* body = new BlockNode({new CallNode(new IdNode("print"), {new IdNode("e")})});
    expected_node = new ForNode("e", new ListNode(list), body);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, div_expression) {
    std::string text = "foo/1";
    SetUp(text);
    node = parser->parse_mul_div_or_mod_expression();
    expected_node = new BinopNode(OpType::DIV, new IdNode("foo"), new NumberNode(1));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_list_empty_no_type_throws_error) {
    std::string text = "[]";
    SetUp(text);
    try {
        node = parser->parse_expression();
        FAIL() << "Did not throw an error";
    } catch (...) {

    }
}

TEST_F(parser_test, parse_xxx) {
    std::string text = "y[2]+x[7] + 43";
    SetUp(text);
    node = parser->parse_expression();
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
    node = parser->parse_expression();
    VectorOfNodes list;
    expected_node = (new EmptyListNode(new T_INT));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_list_one_element) {
    std::string text = "[13]";
    SetUp(text);
    node = parser->parse_expression();
    VectorOfNodes list = {new NumberNode(13)};
    expected_node = new ListNode(list);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_none) {
    std::string text = "none";
    SetUp(text);
    node = parser->parse_expression();
    expected_node = (new NoneNode());
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_list_multiple_elements) {
    std::string text = "[23,17,64]";
    SetUp(text);
    node = parser->parse_expression();
    VectorOfNodes list = {new NumberNode(23), new NumberNode(17), new NumberNode(64)};
    expected_node = new ListNode(list);
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, parse_top_level) {
    std::string text = "var x = 9*7;";
    SetUp(text);
    node = parser->parse_top_level_statement();
    expected_node = new DeclarationNode(
            "x", nullptr,
            new BinopNode(OpType::MUL, new NumberNode(9), new NumberNode(7)));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, complex_div_expression) {
    std::string text = "foo/(1)+a";
    SetUp(text);
    node = parser->parse_add_or_sub_expression();
    expected_node = new BinopNode(
            OpType::ADD, new BinopNode(OpType::DIV, new IdNode("foo"), new NumberNode(1)),
            new IdNode("a"));
    EXPECT_EQ(*node, *expected_node);
}

TEST_F(parser_test, complex_expression) {
    std::string text = "1-(7)*8-(9*(1-3)/7-8+4)+8";
    SetUp(text);
    node = parser->parse_expression();
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
    node = parser->parse_expression();
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