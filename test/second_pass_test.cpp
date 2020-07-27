#include <gtest/gtest.h>
#include <Scanner.h>
#include <Parser.h>
#include <FirstPass.h>
#include <SecondPass.h>

VectorOfNodes get_treeA(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    VectorOfNodes tree = parser.parse_program();
    return tree;
}

#define ASSERT_THROWS_NOT_FOUND_ERROR(NAME) VectorOfNodes tree = get_treeA(text);       \
                                            FirstPass fp;fp.analyze(tree);              \
                                            SecondPass sp(fp.globals, fp.class_table);                  \
                                            try {                                       \
                                                sp.analyze(tree);                       \
                                                FAIL() << "Expected ScopeError thrown"; \
                                            } catch(const ScopeError& se){              \
                                                EXPECT_EQ(se,ScopeError(NAME));         \
                                            }

#define ASSERT_THROWS_REDECLARED_ERROR(NAME) VectorOfNodes tree = get_treeA(text);       \
                                            FirstPass fp;fp.analyze(tree);              \
                                            SecondPass sp(fp.globals, fp.class_table);                  \
                                            try {                                       \
                                                sp.analyze(tree);                       \
                                                FAIL() << "Expected RedeclareError thrown"; \
                                            } catch(const RedeclareError& se){              \
                                                EXPECT_EQ(se,RedeclareError(NAME))  << se.what();       \
                                            }

#define ASSERT_THROWS_RETURN_TYPE_ERROR(NAME, ACTUAL_TYPE, EXPECTED_TYPE) VectorOfNodes tree = get_treeA(text);       \
                                            FirstPass fp;fp.analyze(tree);              \
                                            SecondPass sp(fp.globals, fp.class_table);                  \
                                            try {                                       \
                                                sp.analyze(tree);                       \
                                                FAIL() << "Expected ReturnError thrown"; \
                                            } catch(const ReturnError& se){              \
                                                EXPECT_EQ(se,ReturnError(ACTUAL_TYPE, EXPECTED_TYPE))  << se.what();       }\

#define ASSERT_THROWS_BAD_ARGUMENTS() VectorOfNodes tree = get_treeA(text);       \
                                            FirstPass fp;fp.analyze(tree);              \
                                            SecondPass sp(fp.globals, fp.class_table);                  \
                                            try {                                       \
                                                sp.analyze(tree);                       \
                                                FAIL() << "Expected BadArguments thrown"; \
                                            } catch(const BadArguments& se){              \
                                                EXPECT_EQ(se,BadArguments())  << se.what();       }\

#define ASSERT_OK() VectorOfNodes tree = get_treeA(text);   \
                    FirstPass fp;fp.analyze(tree);          \
                    SecondPass sp(fp.globals, fp.class_table);              \
                    sp.analyze(tree);

TEST(semantic_test, fun_foo_cAomplete) {
    std::string text = "fun foo(y: Foo)->Integer{}";
    VectorOfNodes tree = get_treeA(text);
    FirstPass fp;
    fp.analyze(tree);
    SecondPass sp(fp.globals, fp.class_table);
    sp.analyze(tree);
    SymbolTable* foo_scope = sp.scopes["global.foo"];
    SymbolInfo* sinfo = foo_scope->get("y");
    EXPECT_EQ(sinfo->type, SINFO::SIMPLE);
    EXPECT_EQ(sinfo->object_info->parent, "Foo");
}

TEST(semantic_test, tee) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(semantic_test, teea) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(semantic_test, teeas) {
    std::string text = "fun foo(y: Foo)->String{if(z==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("z");
}


TEST(semantic_test, FOFOO) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){if(y==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(semantic_test, FOFOOa) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){if(z==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("z");
}

TEST(semantic_test, FOFOaOa) {
    std::string text = "fun foo(y: Foo)->String{var x:Integer;}";
    VectorOfNodes tree = get_treeA(text);
    FirstPass fp;
    fp.analyze(tree);
    SecondPass sp(fp.globals, fp.class_table);
    sp.analyze(tree);
    EXPECT_TRUE(sp.scopes["global.foo"]->declared("x"));
}


TEST(semantic_test, z_not_found_error) {
    std::string text = "fun foo(y: Foo)->String{var x:Integer;if(y==1){if(z==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("z");
}

TEST(semantic_test, x_redeclare_in_inner_scope_ok) {
    std::string text = "fun foo(y: Integer)->Integer{var x:Integer;if(y==1){if(y==2){var x:Integer; return x;}}}";
    ASSERT_OK();
}

TEST(semantic_test, x_redeclare_in_same_scope_error) {
    std::string text = "fun foo(y: Foo)->String{var x:Integer;var x:String; return x;}";
    ASSERT_THROWS_REDECLARED_ERROR("x");
}

TEST(semantic_test, x_declare_in_inner_scope_and_use_outside_error) {
    std::string text = "fun foo(y: Foo)->Integer{if(y==1){if(y==2){var x:Integer;return x;}}return x;}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(semantic_test, member_without_this_error) {
    std::string text = "class Foo{var x: String; fun foo()->String{return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(semantic_test, member_with_this_no_error) {
    std::string text = "class Foo{var x: String; fun foo()->String{return this.x;}}";
    ASSERT_OK();
}

TEST(semantic_test, member_not_found_error) {
    std::string text = "class Foo{var x: String; fun foo()->String{return this.y;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("y");
}

TEST(semantic_test, function_return_type_error) {
    std::string text = "fun foo()->String{} fun main()->Integer{return foo();}";
    ASSERT_THROWS_RETURN_TYPE_ERROR("main", "String", "Integer");
}

TEST(semantic_test, function_return_type_ok) {
    std::string text = "fun foo()->String{} fun main()->String{return foo();}";
    ASSERT_OK();
}

TEST(semantic_test, function_argument_type_ok) {
    std::string text = "fun foo(x: Integer) -> Integer {return x;} fun bar()->Integer{var y: Integer; return foo(y);}";
    ASSERT_OK();
}

TEST(semantic_test, function_argument_type_error) {
    std::string text = "fun foo(x: Integer) -> Integer {return x;} fun bar()->Integer{var y: String; return foo(y);}";
    ASSERT_THROWS_BAD_ARGUMENTS();
}

TEST(semantic_test, declaration_type_error_1) {
    std::string text = "var x: String = 5;";
    ASSERT_THROWS_RETURN_TYPE_ERROR("", "Integer", "String");
}

TEST(semantic_test, declaration_type_error_2) {
    std::string text = "var x: Integer = \"Hello\";";
    ASSERT_THROWS_RETURN_TYPE_ERROR("", "String", "Integer");
}

TEST(semantic_test, assignment_type_error_1) {
    std::string text = "var x: String; x = 5;";
    ASSERT_THROWS_RETURN_TYPE_ERROR("", "Integer", "String");
}

TEST(semantic_test, assignment_type_error_2) {
    std::string text = "var x: Integer;x = \"Hello\";";
    ASSERT_THROWS_RETURN_TYPE_ERROR("", "String", "Integer");
}