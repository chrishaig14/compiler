#include <gtest/gtest.h>
#include <nodes/BlockNode.h>
#include <parser/Parser.h>
#include <scanner/Scanner.h>
#include <vm/Object.h>
#include <semantic/GlobalProcessor.h>
#include <semantic/Checker.h>
#include <transpiler/Transpiler.h>
#include "utils_test.h"

class transpiler_test : public ::testing::Test {
protected:
    BlockNode* tree;
    Transpiler t;

    void SetUp(std::string text) {
        Scanner scanner(text);
        std::vector<Token> tokens = scanner.scan_all();
        Token token;
        for (auto token: tokens) {
            std::cout << token.to_string() << std::endl;
        }
        Parser parser(tokens);
        try {
            tree = parser.parse_program();
        } catch (const UnexpectedToken& ut) {
            std::cerr << ut.what() << std::endl;
            exit(1);
        }
        std::vector<Builtin> builtins;

        try {
            GlobalProcessor gp(builtins);
            gp.visit_block(*tree);
            Checker checker(gp.globals, gp.global_classes, gp.global_functions);
            checker.visit_block(*tree);
        } catch (const std::runtime_error& e) {
            std::cerr << "THERE WAS A SEMANTIC ERROR: " << e.what() << std::endl;
            exit(1);
        }
    }

    void TearDown() override {
        delete tree;
    }
};


TEST_F(transpiler_test, test_1) {
    std::string code = "fun main()->Integer{return 5;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "Object* id_main(){return new IntegerObject(5);}";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_2) {
    std::string code = "fun main()->Integer{print(\"Hello\");return 5;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "Object* id_main(){id_print(new StringObject(\"Hello\"));return new IntegerObject(5);}";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_3) {
    std::string code = "fun main()->Integer{while(1 < 7){print(\"Hello\");}return 5;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "Object* id_main(){while(op_lt(new IntegerObject(1), new IntegerObject(7))){id_print(new StringObject(\"Hello\"));}return new IntegerObject(5);}";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_4) {
    std::string code = "fun main()->Integer{var x = 7;return 5;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "Object* id_main(){Object* id_x = new IntegerObject(7);return new IntegerObject(5);}";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_5) {
    std::string code = "fun main()->Integer{var x = 7 + 3;return 5;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "Object* id_main(){Object* id_x = op_add(new IntegerObject(7), new IntegerObject(3));return new IntegerObject(5);}";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_if) {
    std::string code = "fun main()->Integer{var x = 9; if x+7 < 20 {print(\"Less than 20\");}return 5;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "Object* id_main(){Object* id_x = new IntegerObject(9);if(op_lt(op_add(id_x, new IntegerObject(7)), new IntegerObject(20))){id_print(new StringObject(\"Less than 20\"));}return new IntegerObject(5);}";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_list) {
    std::string code = "fun main()->Integer{var x = [3,1,4,2];return 5;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "Object* id_main(){Object* id_x = new ListObject({new IntegerObject(3), new IntegerObject(1), new IntegerObject(4), new IntegerObject(2)});return new IntegerObject(5);}";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_subscript) {
    std::string code = "fun main()->Integer{var x = [3];var l = x[0];return 5;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "Object* id_main(){Object* id_x = new ListObject({new IntegerObject(3)});Object* id_l = subscript(id_x, new IntegerObject(0));return new IntegerObject(5);}";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_class) {
    std::string code = "class Foo {x: Integer;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "struct class_Foo {Object* x;};";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_class_literal_field) {
    std::string code = "class Foo{v: Integer;} fun main()->Integer{var x = #Foo{v: 7}; return 8;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "struct class_Foo {Object* v;};Object* id_main(){Object* id_x = new class_Foo{.v = new IntegerObject(7)};return new IntegerObject(8);}";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_class_with_methods) {
    std::string code = "class Foo {x: Integer; fun foo()->Integer{return 5;}}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "struct class_Foo {Object* x;};Object* id_Foo_foo(){return new IntegerObject(5);}";
    EXPECT_EQ(c_code, e_code);
}

TEST_F(transpiler_test, test_call_method) {
    std::string code = "class Foo {v: Integer; fun foo()->Integer{return 5;}} fun main()->Integer{var x = #Foo{v: 7}; var y = x.foo(); return 3;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "struct class_Foo {Object* v;};Object* id_Foo_foo(class_Foo* this_obj){return new IntegerObject(5);}Object* id_main(){Object* id_x = new class_Foo{.v = new IntegerObject(7)};Object* id_y = id_Foo_foo(id_x);return new IntegerObject(3);}";
    EXPECT_EQ(c_code, e_code);
}


TEST_F(transpiler_test, test_binop) {
    std::string code = "fun main()->Integer{var x = 9; x = x + 1; return 5;}";
    SetUp(code);
    std::string c_code = t.dispatch(this->tree);
    std::string e_code = "Object* id_main(){Object* id_x = new IntegerObject(9);id_x = op_add(id_x, new IntegerObject(1));return new IntegerObject(5);}";
    EXPECT_EQ(c_code, e_code);
}


