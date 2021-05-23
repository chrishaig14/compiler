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
    std::vector <std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit_block(*tree);
    Checker checker(gp.globals, gp.global_classes, gp.global_functions);
    checker.visit_block(*tree);
}

void compile(std::string text) {
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit_block(*tree);
    Checker checker(gp.globals, gp.global_classes, gp.global_functions);
    checker.visit_block(*tree);
}

class global_test : public ::testing::Test {
protected:
    global_test() {
    }

    BlockNode* tree;
    GlobalProcessor gp;

    void SetUp(std::string text) {
        tree = get_ast(text);
    }

    void TearDown() override {
        delete tree;
        delete gp.globals;
        delete gp.global_classes;
        delete gp.global_functions;
    }
};


TEST_F(global_test, test_class_declared_ok) {
    std::string text = "class Foo {x: Integer\ny:String\n}";
    SetUp(text);
    gp.visit_block(*tree);
    EXPECT_TRUE(gp.global_classes->declared("Foo"));
    EXPECT_FALSE(gp.global_classes->declared("Bar"));
}

TEST_F(global_test, test_class_info_members_ok) {
    std::string text = "class Foo {x: Integer\ny:String\n}";
    SetUp(text);
    gp.visit_block(*tree);
    EXPECT_EQ(gp.global_classes->get("Foo")->members.count("x"), 1);
    EXPECT_EQ(gp.global_classes->get("Foo")->members.count("y"), 1);
    EXPECT_EQ(gp.global_classes->get("Foo")->members.count("z"), 0);
    EXPECT_EQ(*gp.global_classes->get("Foo")->members["x"], T_INT);
    EXPECT_EQ(*gp.global_classes->get("Foo")->members["y"], T_STRING);
}

TEST_F(global_test, test_class_info_methods_ok) {
    std::string text = "class Foo {x: Integer\ny:String\nfun foo()->Integer{return this.x\n}\n}";
    SetUp(text);
    gp.visit_block(*tree);
    EXPECT_EQ(gp.global_classes->get("Foo")->methods.count("foo"), 1);
    EXPECT_EQ(*gp.global_classes->get("Foo")->methods["foo"], FunctionType({}, new T_INT));
}

TEST_F(global_test, test_class_info_method_redeclared_error) {
    std::string text = "class Foo {x: Integer\ny:String\nfun foo()->Integer{return this.x\n}\nfun foo(x: Integer)->String{return this.x\n}\n}";
    SetUp(text);
    try {
        gp.visit_block(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(global_test, test_class_already_declared_error) {
    std::string text = "class Foo {x: Integer\ny:String\n}\nclass Foo {x: String\n}";
    SetUp(text);
    try {
        gp.visit_block(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(global_test, test_function_already_declared_error) {
    std::string text = "fun foo()->Integer{}\nfun foo(x: Integer)->String{}";
    SetUp(text);
    try {
        gp.visit_block(*tree);
        FAIL();
    } catch (...) {

    }
}

TEST_F(global_test, test_already_declared_error) {
    std::string text = "class Foo {x: Integer\ny:String\n}\nfun Foo(x: Integer)->String{}";
    SetUp(text);
    try {
        gp.visit_block(*tree);
        FAIL();
    } catch (...) {

    }
}

BlockNode* get_ast(std::string text) {
    Scanner scanner(text);
    std::vector <Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* tree = parser.parse_program();
    return tree;
}
