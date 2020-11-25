#include <gtest/gtest.h>
#include <parser/Parser.h>
#include <scanner/Scanner.h>
#include <semantic/GlobalProcessor.h>
#include <semantic/Checker.h>
#include "vm/CodeObject.h"


BlockNode* get_ast(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* tree = parser.parse_program();
    return tree;
}

void compile(std::string text) {
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}


class checker_test : public ::testing::Test {
protected:
    BlockNode* tree;
    GlobalProcessor gp;
    Checker* checker;

    void SetUp(std::string text) {
        tree = get_ast(text);
        gp.visit(*tree);
        checker = new Checker(gp.globals, gp.class_table, gp.function_table);
    }

    void TearDown() override {
        delete tree;
        delete gp.globals;
        delete gp.class_table;
        delete gp.function_table;
        delete checker;
    }
};


TEST_F(checker_test, fun_foo_complete) {
    std::string text = "fun foo(y: String)->Integer{return 0;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_assign_string_to_int_error) {
    std::string text = "fun foo()->Integer{var x : Integer = \"Hello\";return 0;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_assign_int_to_string_error) {
    std::string text = "fun foo()->Integer{var x : Integer = \"Hello\";return 0;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_assign_int_to_int_ok) {
    std::string text = "fun foo()->Integer{var x : Integer = 123;return 0;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_assign_string_to_string_ok) {
    std::string text = "fun foo()->Integer{var x : String = \"Hello\";return 0;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_if_condition_not_boolean_error) {
    std::string text = "fun foo()->Integer{if 2 {return 1;}return 0;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_if_condition_boolean_ok) {
    std::string text = "fun foo()->Integer{if true {return 1;}return 0;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_tuple_type_assign_ok) {
    std::string text = "fun foo()->Integer{var x = #(4, \"Hello\", false); var y : Tuple[Integer, String, Boolean] = x; return 5;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_tuple_type_assign_error) {
    std::string text = "fun foo()->Integer{var x = #(\"Hello\", false, 4); var y : Tuple[Integer, String, Boolean] = x; return 5;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_call_function_pass_tuple) {
    std::string text = "fun foo(t: Tuple[Integer, String])->Integer{var x = t; return 5;} fun main(){var y = foo(#(5, \"Hola\"));}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_call_function_pass_tuple_error) {
    std::string text = "fun foo(t: Tuple[Integer, String])->Integer{var x = t; return 5;} fun main(){var y = foo(#(5, false));}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_access_tuple_element) {
    std::string text = "fun foo()->Integer{var x = #(1, \"Hello\", false); var y = false; y = x.3; return 5;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_cant_assign_to_tuple_element) {
    std::string text = "fun foo()->Integer{var x = #(1, \"Hello\", false); x.3 = true; return 5;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_cant_assign_to_string_position) {
    std::string text = "fun foo()->Integer{var x = \"Hello\"; x[2] = \"w\";}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_string_subscript_ok) {
    std::string text = "fun foo()->Integer{var x = \"Hello\"; var y : String = x[2]; return 0;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, list_index_float_error) {
    std::string text = "fun foo()->Integer{var x = [3,1,4,1,5]; var y = x[2.5]; return 0;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {
    }
}

TEST_F(checker_test, float_type_ok) {
    std::string text = "fun foo()->Integer{var x : Float = 0.5; return 0;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, float_type_error) {
    std::string text = "fun foo()->Integer{var x : Integer = 0.5; return 0;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {
    }
}

TEST_F(checker_test, test_access_tuple_element_out_of_range_error) {
    std::string text = "fun foo()->Integer{var x = #(1, \"Hello\", false); var y = false; y = x.432; return 5;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_access_tuple_element_string_error) {
    std::string text = "fun foo()->Integer{var x = #(1, \"Hello\", false); var y = false; y = x.foo; return 5;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_while_condition_not_boolean_error) {
    std::string text = "fun foo()->Integer{while 2 {return 1;}return 0;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_while_condition_boolean_ok) {
    std::string text = "fun foo()->Integer{while true {return 1;}return 0;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_for_not_a_list_error) {
    std::string text = "fun foo()->Integer{for x @ 2 {return 1;}return 0;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_for_list_ok) {
    std::string text = "fun foo()->Integer{for x @ [1,2,3] {return 1;}return 0;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_function_return_wrong_type_error) {
    std::string text = "fun foo()->Integer{return \"Hello\";}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_function_return_ok) {
    std::string text = "fun foo()->Integer{return 1;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_call_undefined_function_error) {
    std::string text = "fun foo()->Integer{return bar();}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_call_function_ok) {
    std::string text = "fun foo()->Integer{return bar();}fun bar()->Integer{return 5;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_empty_list_type_ok) {
    std::string text = "fun foo()->Integer{var x : List[Integer] = []::Integer; return 0;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_empty_list_type_error) {
    std::string text = "fun foo()->Integer{var x : List[String] = []::Integer; return 0;}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_add_list_ok) {
    std::string text = "fun foo()->Integer{var x = [0]; x = x + [1]; return 1;}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_declare_class_ok) {
    std::string text = "class Foo{x: Integer\ny: String\n}";
    SetUp(text);
    checker->visit(*tree);
}

TEST_F(checker_test, test_variable_redeclared_error) {
    std::string text = "fun foo(){var x = 0; var x : String = \"Hello\";}";
    SetUp(text);
    try {
        checker->visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(checker_test, test_variable_redeclared_inside_scope_ok) {
    std::string text = "fun foo(){var x = 0; if x == 0 {var x : String = \"Hello\"; var y: String = x;} if x == 1 {var x : Boolean = false; var y : Boolean = x;}}";
    SetUp(text);
    checker->visit(*tree);
}