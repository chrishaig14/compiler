#include <gtest/gtest.h>
#include <nodes/BlockNode.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <semantic/GlobalProcessor.h>
#include <semantic/Checker.h>
#include <macros.h>


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

class global_test : public ::testing::Test {
protected:
    BlockNode* tree;
    GlobalProcessor gp;

    void SetUp(std::string text) {
        tree = get_ast(text);
    }

    void TearDown() override {
        delete tree;
        delete gp.globals;
        delete gp.class_table;
        delete gp.function_table;
    }
};


TEST_F(global_test, test_class_declared_ok) {
    std::string text = "class Foo {x: Integer\ny:String\n}";
    SetUp(text);
    gp.visit(*tree);
    EXPECT_TRUE(gp.class_table->declared("Foo"));
    EXPECT_FALSE(gp.class_table->declared("Bar"));
}

TEST_F(global_test, test_class_info_members_ok) {
    std::string text = "class Foo {x: Integer\ny:String\n}";
    SetUp(text);
    gp.visit(*tree);
    EXPECT_EQ(gp.class_table->get("Foo")->members.count("x"), 1);
    EXPECT_EQ(gp.class_table->get("Foo")->members.count("y"), 1);
    EXPECT_EQ(gp.class_table->get("Foo")->members.count("z"), 0);
    EXPECT_EQ(*gp.class_table->get("Foo")->members["x"], T_INT);
    EXPECT_EQ(*gp.class_table->get("Foo")->members["y"], T_STRING);
}

TEST_F(global_test, test_class_already_declared_error) {
    std::string text = "class Foo {x: Integer\ny:String\n}\nclass Foo {x: String\n}";
    SetUp(text);
    try {
        gp.visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(global_test, test_function_already_declared_error) {
    std::string text = "fun foo()->Integer{}\nfun foo(x: Integer)->String{}";
    SetUp(text);
    try {
        gp.visit(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(global_test, test_already_declared_error) {
    std::string text = "class Foo {x: Integer\ny:String\n}\nfun Foo(x: Integer)->String{}";
    SetUp(text);
    try {
        gp.visit(*tree);
        FAIL();
    } catch (...) {

    }
}
