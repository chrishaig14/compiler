#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <semantic/GlobalProcessor.h>
#include <utils.h>

#define S_INFO(o) new SymbolInfo(o)
#define F_INFO(a, b) new FunctionInfo(a,b)
#define O_INFO(a) new ObjectInfo(a)
#define C_INFO(a, b) new ClassInfo(a,b)

BlockNode* get_tree(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    return parser.parse_program();
}

TEST(first_pass_test, fun_foo) {
    std::string text = "fun foo()->None{}";
    BlockNode* tree = get_tree(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    SymbolInfo* ginfo = gp.globals->get("foo");
    EXPECT_EQ(ginfo->type, SINFO::FUNCTION);
}

TEST(first_pass_test, fun_foo_eq) {
    std::string text = "fun foo()->String{}";
    BlockNode* tree = get_tree(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    SymbolInfo* ginfo = gp.globals->get("foo");
    EXPECT_TRUE(*ginfo == *S_INFO(F_INFO({}, T_STRING)));
}

TEST(first_pass_test, fun_foo_complete) {
    std::string text = "fun foo(y: Integer, x: String)-> Boolean{}";
    BlockNode* tree = get_tree(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    SymbolInfo* ginfo = gp.globals->get("foo");
    EXPECT_TRUE(*ginfo == *S_INFO(F_INFO(std::vector<TypeNode*>({T_INT, T_STRING}), T_BOOL)));
}

TEST(first_pass_test, class_foo) {
    std::string text = "class Foo{}";
    BlockNode* tree = get_tree(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    ClassInfo* ginfo = gp.class_table->get("Foo");
}

TEST(first_pass_test, class_foo_eq) {
    std::string text = "class Foo{}";
    BlockNode* tree = get_tree(text);
    GlobalProcessor gp;
    gp.visit(*tree);
}

typedef std::map<std::string, SymbolInfo*> MapStringToSimple;
typedef std::map<std::string, FunctionInfo*> MapStringToFunction;
TEST(first_pass_test, class_foo_with_field) {
    std::string text = "class Foo{var x: String;}";
    BlockNode* tree = get_tree(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    ClassInfo* ginfo = gp.class_table->get("Foo");
    MapStringToSimple fields;
    fields["x"] = S_INFO(O_INFO(T_STRING));
    ClassInfo class_info;
    class_info.fields = fields;
    EXPECT_TRUE(*ginfo == class_info);
}

TEST(first_pass_test, class_foo_with_method) {
    std::string text = "class Foo{fun foo(x:Integer)->String{}}";
    BlockNode* tree = get_tree(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    ClassInfo* ginfo = gp.class_table->get("Foo");
    MapStringToFunction methods;
    methods["foo"] = F_INFO(VectorOfTypes({T_INT}), T_STRING);
    ClassInfo expected;
    expected.methods = methods;
    EXPECT_TRUE(*ginfo == expected);
}

TEST(first_pass_test, class_foo_complete) {
    std::string text = "class Foo{var y: String; var z: Integer; fun foo(x:Integer)->String{} fun bar(w: List[String], t: Dict[String, Integer])->Integer{}}";
    BlockNode* tree = get_tree(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    ClassInfo* ginfo = gp.class_table->get("Foo");
    MapStringToFunction methods;
    methods["foo"] = F_INFO(VectorOfTypes({T_INT}), T_STRING);
    methods["bar"] = F_INFO(VectorOfTypes({T_LIST(T_STRING), T_DICT(T_STRING, T_INT)}), T_INT);
    MapStringToSimple fields;
    fields["y"] = S_INFO(O_INFO(T_STRING));
    fields["z"] = S_INFO(O_INFO(T_INT));
    ClassInfo expected;
    expected.fields = fields;
    expected.methods = methods;
    EXPECT_TRUE(*ginfo == expected);
}
