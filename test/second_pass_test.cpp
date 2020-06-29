#include <gtest/gtest.h>
#include <Scanner.h>
#include <Parser.h>
#include <FirstPass.h>

VectorOfNodes get_treeA(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    VectorOfNodes tree = parser.parse_program();
    FirstPass fp;
    fp.analyze(tree);
}

TEST(semantic_test, fun_foo_cAomplete) {
    std::string text = "fun foo(y: Foo){}";
    VectorOfNodes tree = get_treeA(text);
    FirstPass fp;
    fp.analyze(tree);
    GeneralInfo* ginfo = fp.globals["foo"];
    FunctionInfo* finfo = ginfo->function_info;
    EXPECT_EQ(ginfo->type, GINFO::FUNCTION);
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
