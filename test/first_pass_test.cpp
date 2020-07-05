#include <gtest/gtest.h>
#include <Scanner.h>
#include <Parser.h>
#include <FirstPass.h>

bool eq_si(SimpleInfo* t1, SimpleInfo* t2) {
    EXPECT_EQ(t1->parent, t2->parent);
    EXPECT_EQ(t1->type_parameters.size(), t2->type_parameters.size());
    for (int i = 0; i < t1->type_parameters.size(); i++) {
        EXPECT_EQ(eq_si(t1->type_parameters[i], t2->type_parameters[i]), true);
    }
}

VectorOfNodes get_tree(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    return parser.parse_program();
}

TEST(semantic_test, fun_foo) {
    std::string text = "fun foo(){}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("foo");
    FunctionInfo* finfo = ginfo->function_info;
    EXPECT_EQ(ginfo->type, SINFO::FUNCTION);
}

TEST(semantic_test, fun_foo_eq) {
    std::string text = "fun foo(){}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("foo");
    EXPECT_EQ(ginfo->type, SINFO::FUNCTION);
    FunctionInfo* finfo = ginfo->function_info;
    EXPECT_EQ(finfo->parameter_types.size(), 0);
    EXPECT_EQ(finfo->return_type, nullptr);
}

TEST(semantic_test, fun_foo_complete) {
    std::string text = "fun foo(y: Integer, x: String): Boolean{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("foo");
    FunctionInfo* finfo = ginfo->function_info;
    EXPECT_EQ(ginfo->type, SINFO::FUNCTION);

    EXPECT_EQ(finfo->parameter_types.size(), 2);
    SimpleInfo* yinfo = finfo->parameter_types[0];
    EXPECT_EQ(yinfo->parent, "Integer");
    EXPECT_EQ(yinfo->type_parameters.size(), 0);
    SimpleInfo* xinfo = finfo->parameter_types[1];
    EXPECT_EQ(xinfo->parent, "String");
    EXPECT_EQ(xinfo->type_parameters.size(), 0);
    SimpleInfo* ret_info = finfo->return_type;
    EXPECT_EQ(ret_info->parent, "Boolean");
    EXPECT_EQ(ret_info->type_parameters.size(), 0);
}

TEST(semantic_test, class_foo) {
    std::string text = "class Foo{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("Foo");
    EXPECT_EQ(ginfo->type, SINFO::CLASS);
}

TEST(semantic_test, class_foo_eq) {
    std::string text = "class Foo{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("Foo");
    ClassInfo* cinfo = ginfo->class_info;
    EXPECT_EQ(cinfo->methods.size(), 0);
    EXPECT_EQ(cinfo->fields.size(), 0);
}

TEST(semantic_test, class_foo_with_field) {
    std::string text = "class Foo{var x: String;}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("Foo");
    ClassInfo* cinfo = ginfo->class_info;
    EXPECT_EQ(cinfo->methods.size(), 0);
    EXPECT_EQ(cinfo->fields.size(), 1);
    SimpleInfo* xinfo = cinfo->fields["x"];
    EXPECT_EQ(xinfo->parent, "String");
    EXPECT_EQ(xinfo->type_parameters.size(), 0);
}

TEST(semantic_test, class_foo_with_method) {
    std::string text = "class Foo{fun foo(x:Integer):String{}}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("Foo");
    ClassInfo* cinfo = ginfo->class_info;
    EXPECT_EQ(cinfo->methods.size(), 1);
    EXPECT_EQ(cinfo->fields.size(), 0);
    FunctionInfo* foo_info = cinfo->methods["foo"];
    EXPECT_EQ(foo_info->parameter_types.size(), 1);
    SimpleInfo* xinfo = foo_info->parameter_types[0];
    EXPECT_EQ(xinfo->parent, "Integer");
    EXPECT_EQ(xinfo->type_parameters.size(), 0);
    SimpleInfo* ret_info = foo_info->return_type;
    EXPECT_EQ(ret_info->parent, "String");
    EXPECT_EQ(ret_info->type_parameters.size(), 0);
}