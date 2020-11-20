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

void ASSERT_THROWS_NOT_FOUND_ERROR(std::string text, std::string NAME) {
    try {
        compile(text);
        FAIL() << "Expected ScopeError thrown";
    } catch (const ScopeError& se) {
        EXPECT_EQ(se, ScopeError(NAME));
    }
}


void ASSERT_THROWS_REDECLARED_ERROR(std::string text, std::string NAME) {
    try {
        compile(text);
        FAIL() << "Expected RedeclareError thrown";
    } catch (const RedeclareError& se) {
        EXPECT_EQ(se, RedeclareError(NAME)) << se.what();
    }
}

void
ASSERT_THROWS_RETURN_TYPE_ERROR(std::string text, std::string NAME, TypeNode& EXPECTED_TYPE, TypeNode& ACTUAL_TYPE) {
    try {
        compile(text);
        FAIL() << "Expected ReturnError thrown";
    } catch (const ReturnError& se) {
        EXPECT_EQ(se, ReturnError(EXPECTED_TYPE, ACTUAL_TYPE)) << se.what();
    }
}

void ASSERT_THROWS_ASSIGNMENT_ERROR(std::string text, TypeNode& EXPECTED_TYPE, TypeNode& ACTUAL_TYPE) {
    try {
        compile(text);
        FAIL() << "Expected AssignmentTypeError thrown";
    } catch (const AssignmentTypeError& se) {
        EXPECT_EQ(se, AssignmentTypeError(EXPECTED_TYPE, ACTUAL_TYPE)) << se.what();
    }
}

void ASSERT_THROWS_BAD_ARGUMENTS(std::string text) {
    try {
        compile(text);
        FAIL() << "Expected BadArguments thrown";
    } catch (const BadArguments& se) {
    }
}

void ASSERT_OK(std::string text) { compile(text); }

void ASSERT_FAILS(std::string text) {
    try {
        compile(text);
        FAIL() << "Expected exception!";
    } catch (...) {
    }
}


void ASSERT_VARIABLE_TYPE(std::string text, std::string id, TypeNode* type) {
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
    EXPECT_EQ(checker.scopes["global"]->get(id), *type);
}

TEST(checker_test, fun_foo_cAomplete) {
    std::string text = "fun foo(y: String)->Integer{return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}

TEST(checker_test, test_assign_string_to_int_error) {
    std::string text = "fun foo()->Integer{var x : Integer = \"Hello\";return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    try {
        checker.visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST(checker_test, test_assign_int_to_string_error) {
    std::string text = "fun foo()->Integer{var x : Integer = \"Hello\";return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    try {
        checker.visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST(checker_test, test_assign_int_to_int_ok) {
    std::string text = "fun foo()->Integer{var x : Integer = 123;return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}

TEST(checker_test, test_assign_string_to_string_ok) {
    std::string text = "fun foo()->Integer{var x : String = \"Hello\";return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}

TEST(checker_test, test_if_condition_not_boolean_error) {
    std::string text = "fun foo()->Integer{if 2 {return 1;}return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    try {
        checker.visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST(checker_test, test_if_condition_boolean_ok) {
    std::string text = "fun foo()->Integer{if true {return 1;}return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}

TEST(checker_test, test_while_condition_not_boolean_error) {
    std::string text = "fun foo()->Integer{while 2 {return 1;}return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    try {
        checker.visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST(checker_test, test_while_condition_boolean_ok) {
    std::string text = "fun foo()->Integer{while true {return 1;}return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}

TEST(checker_test, test_for_not_a_list_error) {
    std::string text = "fun foo()->Integer{for x @ 2 {return 1;}return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    try {
        checker.visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST(checker_test, test_for_list_ok) {
    std::string text = "fun foo()->Integer{for x @ [1,2,3] {return 1;}return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}

TEST(checker_test, test_function_return_wrong_type_error) {
    std::string text = "fun foo()->Integer{return \"Hello\";}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    try {
        checker.visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST(checker_test, test_function_return_ok) {
    std::string text = "fun foo()->Integer{return 1;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}

TEST(checker_test, test_call_undefined_function_error) {
    std::string text = "fun foo()->Integer{return bar();}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    try {
        checker.visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST(checker_test, test_call_function_ok) {
    std::string text = "fun foo()->Integer{return bar();}fun bar()->Integer{return 5;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}

TEST(checker_test, test_empty_list_type_ok) {
    std::string text = "fun foo()->Integer{var x : List[Integer] = []::Integer; return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}

TEST(checker_test, test_empty_list_type_error) {
    std::string text = "fun foo()->Integer{var x : List[String] = []::Integer; return 0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    try {
        checker.visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST(checker_test, test_add_list_ok) {
    std::string text = "fun foo()->Integer{var x = [0]; x = x + [1]; return 1;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table, gp.function_table);
    checker.visit(*tree);
}

void f(CodeBuiltin c) {
    FunctionTypeNode* ft = new FunctionTypeNode({new ObjectTypeNode("foo", {})}, *new ObjectTypeNode("Integer", {}));
    CodeBuiltin d = {ft, nullptr};
    d = c;
}