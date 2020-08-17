#include <gtest/gtest.h>
#include <parser/Parser.h>
#include <scanner/Scanner.h>
#include <semantic/GlobalProcessor.h>
#include <semantic/Checker.h>

BlockNode* get_treeA(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* tree = parser.parse_program();
    return tree;
}

#define ASSERT_THROWS_NOT_FOUND_ERROR(NAME) BlockNode* tree = get_treeA(text);       \
                                            GlobalProcessor gp;gp.visit(*tree);              \
                                            Checker checker(gp.globals, gp.class_table);                  \
                                            try {                                       \
                                                checker.visit(*tree);                       \
                                                FAIL() << "Expected ScopeError thrown"; \
                                            } catch(const ScopeError& se){              \
                                                EXPECT_EQ(se,ScopeError(NAME));         \
                                            }

#define ASSERT_THROWS_REDECLARED_ERROR(NAME) BlockNode* tree = get_treeA(text);       \
                                            GlobalProcessor gp;gp.visit(*tree);              \
                                            Checker checker(gp.globals, gp.class_table);                  \
                                            try {                                       \
                                                checker.visit(*tree);                       \
                                                FAIL() << "Expected RedeclareError thrown"; \
                                            } catch(const RedeclareError& se){              \
                                                EXPECT_EQ(se,RedeclareError(NAME))  << se.what();       \
                                            }

#define ASSERT_THROWS_RETURN_TYPE_ERROR(NAME, EXPECTED_TYPE, ACTUAL_TYPE) BlockNode* tree = get_treeA(text);       \
                                            GlobalProcessor gp;gp.visit(*tree);              \
                                            Checker checker(gp.globals, gp.class_table);                  \
                                            try {                                       \
                                                checker.visit(*tree);                       \
                                                FAIL() << "Expected ReturnError thrown"; \
                                            } catch(const ReturnError& se){              \
                                                EXPECT_EQ(se,ReturnError(EXPECTED_TYPE, ACTUAL_TYPE))  << se.what();       }\

#define ASSERT_THROWS_ASSIGNMENT_ERROR(EXPECTED_TYPE, ACTUAL_TYPE ) BlockNode* tree = get_treeA(text);       \
                                            GlobalProcessor gp;gp.visit(*tree);              \
                                            Checker checker(gp.globals, gp.class_table);                  \
                                            try {                                       \
                                                checker.visit(*tree);                       \
                                                FAIL() << "Expected AssignmentTypeError thrown"; \
                                            } catch(const AssignmentTypeError& se){              \
                                                EXPECT_EQ(se,AssignmentTypeError(EXPECTED_TYPE, ACTUAL_TYPE))  << se.what();       }\


#define ASSERT_THROWS_BAD_ARGUMENTS() BlockNode* tree = get_treeA(text);       \
                                            GlobalProcessor gp;gp.visit(*tree);              \
                                            Checker checker(gp.globals, gp.class_table);                  \
                                            try {                                       \
                                                checker.visit(*tree);                       \
                                                FAIL() << "Expected BadArguments thrown"; \
                                            } catch(const BadArguments& se){              \
                                                EXPECT_EQ(se,BadArguments())  << se.what();       }\

#define ASSERT_OK() BlockNode* tree = get_treeA(text);   \
                    GlobalProcessor gp;gp.visit(*tree);          \
                    Checker checker(gp.globals, gp.class_table);              \
                    checker.visit(*tree);

#define ASSERT_VARIABLE_TYPE(id, type) BlockNode* tree = get_treeA(text);   \
                    GlobalProcessor gp;gp.visit(*tree);          \
                    Checker checker(gp.globals, gp.class_table);              \
                    checker.visit(*tree);\
                    EXPECT_TRUE(checker.scopes["global"]->get(id)->equal(type));

TEST(second_pass_test, fun_foo_cAomplete) {
    std::string text = "fun foo(y: Foo)->Integer{}";
    BlockNode* tree = get_treeA(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    SymbolTable* foo_scope = checker.scopes["global.foo"];
    ObjectTypeNode* sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_NE(sinfo, nullptr);
    EXPECT_TRUE(((FunctionNode*) tree->nodes[0])->free_variables.size() == 0);
    EXPECT_EQ(sinfo->identifier, "Foo");
}

TEST(second_pass_test, free_variable_test_1) {
    std::string text = "var x: Integer=0; fun foo(y: Foo)->Integer{return x;}";
    BlockNode* tree = get_treeA(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->has("x"));
    SymbolTable* foo_scope = checker.scopes["global.foo"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.size() == 1);
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.count("x") == 1);
    EXPECT_NE(sinfo, nullptr);
    EXPECT_EQ(sinfo->identifier, "Foo");
}


TEST(second_pass_test, free_variable_test_2) {
    std::string text = "var x: Integer=0; fun foo(y: Foo)->Integer{var z: Integer = 1 + x;}";
    BlockNode* tree = get_treeA(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->has("x"));
    SymbolTable* foo_scope = checker.scopes["global.foo"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.size() == 1);
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.count("x") == 1);
    EXPECT_NE(sinfo, nullptr);
    EXPECT_EQ(sinfo->identifier, "Foo");
}

TEST(second_pass_test, free_variable_test_3) {
    std::string text = "var x: Integer=0; fun foo(y: Foo)->Integer{if(y == 3){var z: Integer = 1 + x;}}";
    BlockNode* tree = get_treeA(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->has("x"));
    SymbolTable* foo_scope = checker.scopes["global.foo"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.size() == 1);
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.count("x") == 1);
    EXPECT_NE(sinfo, nullptr);
    EXPECT_EQ(sinfo->identifier, "Foo");
}

TEST(second_pass_test, free_variable_test_4) {
    std::string text = "var x: Integer=0; fun foo(y: Foo)->Integer{if(y == 3){var z: Integer = 1 + y;}}";
    BlockNode* tree = get_treeA(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->has("x"));
    SymbolTable* foo_scope = checker.scopes["global.foo"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_NE(sinfo, nullptr);
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.size() == 0);
    EXPECT_EQ(sinfo->identifier, "Foo");
}

TEST(second_pass_test, tee) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(second_pass_test, teea) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(second_pass_test, teeas) {
    std::string text = "fun foo(y: Foo)->String{if(z==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("z");
}


TEST(second_pass_test, FOFOO) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){if(y==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(second_pass_test, FOFOOa) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){if(z==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("z");
}

TEST(second_pass_test, FOFOaOa) {
    std::string text = "fun foo(y: Foo)->String{var x:Integer=0;}";
    BlockNode* tree = get_treeA(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global.foo"]->declared("x"));
}


TEST(second_pass_test, z_not_found_error) {
    std::string text = "fun foo(y: Foo)->String{var x:Integer=0;if(y==1){if(z==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("z");
}

TEST(second_pass_test, x_redeclare_in_inner_scope_ok) {
    std::string text = "fun foo(y: Integer)->Integer{var x:Integer=0;if(y==1){if(y==2){var x:Integer=7; return x;}}}";
    ASSERT_OK();
}

TEST(second_pass_test, x_redeclare_in_same_scope_error) {
    std::string text = "fun foo(y: Foo)->String{var x:Integer=0;var x:String=\"\"; return x;}";
    ASSERT_THROWS_REDECLARED_ERROR("x");
}

TEST(second_pass_test, x_declare_in_inner_scope_and_use_outside_error) {
    std::string text = "fun foo(y: Foo)->Integer{if(y==1){if(y==2){var x:Integer=0;return x;}}return x;}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(second_pass_test, function_return_type_error) {
    std::string text = "fun foo()->String{} fun main()->Integer{return foo();}";
    ASSERT_THROWS_RETURN_TYPE_ERROR("main", T_STRING, T_INT);
}

TEST(second_pass_test, function_return_type_ok) {
    std::string text = "fun foo()->String{} fun main()->String{return foo();}";
    ASSERT_OK();
}

TEST(second_pass_test, function_argument_type_ok) {
    std::string text = "fun foo(x: Integer) -> Integer {return x;} fun bar()->Integer{var y: Integer=0; return foo(y);}";
    ASSERT_OK();
}

TEST(second_pass_test, function_argument_type_error) {
    std::string text = "fun foo(x: Integer) -> Integer {return x;} fun bar()->Integer{var y: String=\"\"; return foo(y);}";
    ASSERT_THROWS_BAD_ARGUMENTS();
}

TEST(second_pass_test, declaration_type_error_1) {
    std::string text = "var x: String = 5;";
    ASSERT_THROWS_ASSIGNMENT_ERROR(T_STRING, T_INT);
}

TEST(second_pass_test, declaration_type_error_2) {
    std::string text = "var x: Integer = \"Hello\";";
    ASSERT_THROWS_ASSIGNMENT_ERROR(T_INT, T_STRING);

}

TEST(second_pass_test, assignment_type_error_1) {
    std::string text = "var x: String=\"\"; x = 5;";
    ASSERT_THROWS_ASSIGNMENT_ERROR(T_STRING, T_INT);
}

TEST(second_pass_test, assignment_type_error_2) {
    std::string text = "var x: Integer=0;x = \"Hello\";";
    ASSERT_THROWS_ASSIGNMENT_ERROR(T_INT, T_STRING);
}

TEST(second_pass_test, for_1) {
    std::string text = "var y = 0; for(x:[1,2,3,4]){y = x;}";
    ASSERT_OK();
}

TEST(second_pass_test, for_2) {
    std::string text = "var y = 0; for(x:[1,2,3,4]){y = z;}";
    ASSERT_THROWS_NOT_FOUND_ERROR("z");
}

TEST(second_pass_test, for_3) {
    std::string text = "var y = 0; for(x:[1,2,3,4]){y = x;} y = x;";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(second_pass_test, for_4) {
    std::string text = "var y = 0; for(x:[1,2,3,4]){y = x;var w = 0;} y = w;";
    ASSERT_THROWS_NOT_FOUND_ERROR("w");
}

TEST(second_pass_test, class_literal_expression_ok) {
    std::string text = "struct Foo{name: String;} var f = Foo{\"Hello\"};";
    ASSERT_OK();
}

TEST(second_pass_test, class_literal_expression_error) {
    std::string text = "struct Foo{name: String;} var f = Foo{25};";
    ASSERT_OK();
}

TEST(second_pass_test, infer_boolean_false) {
    std::string text = "var x = false;";
    ASSERT_VARIABLE_TYPE("x",T_BOOL);
}

TEST(second_pass_test, infer_boolean_true) {
    std::string text = "var x = true;";
    ASSERT_VARIABLE_TYPE("x",T_BOOL);
}