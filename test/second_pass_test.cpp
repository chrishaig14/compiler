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
                                            SecondPass sp(fp.globals);                  \
                                            try {                                       \
                                                sp.analyze(tree);                       \
                                                FAIL() << "Expected ScopeError thrown"; \
                                            } catch(const ScopeError& se){              \
                                                EXPECT_EQ(se,ScopeError(NAME));         \
                                            }

#define ASSERT_THROWS_REDECLARED_ERROR(NAME) VectorOfNodes tree = get_treeA(text);       \
                                            FirstPass fp;fp.analyze(tree);              \
                                            SecondPass sp(fp.globals);                  \
                                            try {                                       \
                                                sp.analyze(tree);                       \
                                                FAIL() << "Expected RedeclareError thrown"; \
                                            } catch(const RedeclareError& se){              \
                                                EXPECT_EQ(se,RedeclareError(NAME))  << se.what();       \
                                            }

#define ASSERT_OK() VectorOfNodes tree = get_treeA(text);   \
                    FirstPass fp;fp.analyze(tree);          \
                    SecondPass sp(fp.globals);              \
                    sp.analyze(tree);

TEST(semantic_test, fun_foo_cAomplete) {
    std::string text = "fun foo(y: Foo){}";
    VectorOfNodes tree = get_treeA(text);
    FirstPass fp;
    fp.analyze(tree);
    SecondPass sp(fp.globals);
    sp.analyze(tree);
    SymbolTable* foo_scope = sp.scopes["global.foo"];
    SymbolInfo* sinfo = foo_scope->get("y");
    EXPECT_EQ(sinfo->type, SINFO::SIMPLE);
    EXPECT_EQ(sinfo->simple_info->parent, "Foo");
}

TEST(semantic_test, tee) {
    std::string text = "fun foo(y: Foo){if(y==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(semantic_test, teea) {
    std::string text = "fun foo(y: Foo){if(y==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(semantic_test, teeas) {
    std::string text = "fun foo(y: Foo){if(z==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("z");
}


TEST(semantic_test, FOFOO) {
    std::string text = "fun foo(y: Foo){if(y==1){if(y==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(semantic_test, FOFOOa) {
    std::string text = "fun foo(y: Foo){if(y==1){if(z==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("z");
}

TEST(semantic_test, FOFOaOa) {
    std::string text = "fun foo(y: Foo){var x:Integer;}";
    VectorOfNodes tree = get_treeA(text);
    FirstPass fp;
    fp.analyze(tree);
    SecondPass sp(fp.globals);
    sp.analyze(tree);
    EXPECT_TRUE(sp.scopes["global.foo"]->declared("x"));
}



TEST(semantic_test, z_not_found_error) {
    std::string text = "fun foo(y: Foo){var x:Integer;if(y==1){if(z==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("z");
}

TEST(semantic_test, x_redeclare_in_inner_scope_ok) {
    std::string text = "fun foo(y: Foo){var x:Integer;if(y==1){if(y==2){var x:Integer; return x;}}}";
    ASSERT_OK();
}

TEST(semantic_test, x_redeclare_in_same_scope_error) {
    std::string text = "fun foo(y: Foo){var x:Integer;return x;var x:String;}";
    ASSERT_THROWS_REDECLARED_ERROR("x");
}

TEST(semantic_test, x_declare_in_inner_scope_and_use_outside_error) {
    std::string text = "fun foo(y: Foo){if(y==1){if(y==2){var x:Integer;return x;}}return x;}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(semantic_test, member_without_this_error) {
    std::string text = "class Foo{var x: String; fun foo(){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("x");
}

TEST(semantic_test, member_with_this_no_error) {
    std::string text = "class Foo{var x: String; fun foo(){return this.x;}}";
    ASSERT_OK();
}

TEST(semantic_test, member_not_found_error) {
    std::string text = "class Foo{var x: String; fun foo(){return this.y;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR("y");
}