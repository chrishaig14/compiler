#include <gtest/gtest.h>
#include <Scanner.h>
#include <Parser.h>
#include <FirstPass.h>
#include <SecondPass.h>

void assert_eq_si(ObjectInfo* t1, ObjectInfo* t2) {
    EXPECT_EQ(t1->parent, t2->parent);
    EXPECT_EQ(t1->type_parameters.size(), t2->type_parameters.size());
    for (int i = 0; i < t1->type_parameters.size(); i++) {
        assert_eq_si(t1->type_parameters[i], t2->type_parameters[i]);
    }
}

VectorOfNodes get_tree(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    return parser.parse_program();
}

TEST(semantic_test, fun_foo) {
    std::string text = "fun foo()->None{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("foo");
    EXPECT_EQ(ginfo->type, SINFO::FUNCTION);
}

TypeNode* typenode_integer = i_type("Integer", {});
TypeNode* typenode_string = i_type("String", {});
TypeNode* typenode_boolean = i_type("Boolean", {});

TypeNode* typenode_list(TypeNode* el) {
    return i_type("List", {el});
}

TypeNode* typenode_dict(TypeNode* k, TypeNode* v) {
    return i_type("Dict", {k, v});
}

TEST(semantic_test, fun_foo_eq) {
    std::string text = "fun foo()->String{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("foo");
    EXPECT_TRUE(equal(ginfo, w_finfo({}, typenode_string)));
}

TEST(semantic_test, fun_foo_complete) {
    std::string text = "fun foo(y: Integer, x: String)-> Boolean{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("foo");
    EXPECT_TRUE(equal(ginfo, w_finfo({typenode_integer, typenode_string}, typenode_boolean)));
}

TEST(semantic_test, class_foo) {
    std::string text = "class Foo{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    ClassInfo* ginfo = fp.class_table->get("Foo");
}

TEST(semantic_test, class_foo_eq) {
    std::string text = "class Foo{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
//    ClassInfo* ginfo = fp.class_table->get("Foo");
//    ClassInfo* expected = new ClassInfo;
//    expected->fields = MapStringToSimple();
//    expected->methods = MapStringToFunction();
//    EXPECT_TRUE(equal(ginfo, expected));
}

TEST(semantic_test, class_foo_with_field) {
    std::string text = "class Foo{var x: String;}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    ClassInfo* ginfo = fp.class_table->get("Foo");
    MapStringToSimple fields;
    fields["x"] = s_info(typenode_string);
    ClassInfo* class_info = new ClassInfo;
    class_info->fields = fields;
    class_info->methods;
    EXPECT_TRUE(equal(ginfo, class_info));
}

TEST(semantic_test, class_foo_with_method) {
    std::string text = "class Foo{fun foo(x:Integer)->String{}}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    ClassInfo* ginfo = fp.class_table->get("Foo");
    MapStringToFunction methods;
    methods["foo"] = f_info({typenode_integer}, typenode_string);
    ClassInfo* expected = new ClassInfo;
    expected->methods = methods;
    EXPECT_TRUE(equal(ginfo, expected));
}

TEST(semantic_test, class_foo_complete) {
    std::string text = "class Foo{var y: String; var z: Integer; fun foo(x:Integer)->String{} fun bar(w: List[String], t: Dict[String, Integer])->Integer{}}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    ClassInfo* ginfo = fp.class_table->get("Foo");
    MapStringToFunction methods;
    methods["foo"] = f_info({typenode_integer}, typenode_string);
    methods["bar"] = f_info({typenode_list(typenode_string), typenode_dict(typenode_string, typenode_integer)},
                            typenode_integer);
    MapStringToSimple fields;
    fields["y"] = s_info(typenode_string);
    fields["z"] = s_info(typenode_integer);
    ClassInfo* expected = new ClassInfo;
    expected->fields = fields;
    expected->methods = methods;
    EXPECT_TRUE(equal(ginfo, expected));
}
