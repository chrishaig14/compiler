#include <gtest/gtest.h>
#include <Scanner.h>
#include <Parser.h>
#include <FirstPass.h>
#include <SecondPass.h>
void assert_eq_si(SimpleInfo* t1, SimpleInfo* t2) {
    EXPECT_EQ(t1->parent, t2->parent);
    EXPECT_EQ(t1->type_parameters.size(), t2->type_parameters.size());
    for (int i = 0; i < t1->type_parameters.size(); i++) {
        assert_eq_si(t1->type_parameters[i], t2->type_parameters[i]);
    }
}

bool both_null(void* a, void* b) {
    return a == nullptr and b == nullptr;
}

bool one_null(void* a, void* b) {
    return a == nullptr or b == nullptr;
}

bool equal(SimpleInfo* a, SimpleInfo* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (a->parent == b->parent) {
        if (a->type_parameters.size() != b->type_parameters.size()) return false;
        for (int i = 0; i < a->type_parameters.size(); i++) {
            if (!equal(a->type_parameters[i], b->type_parameters[i]))return false;
        }
    }
    return true;
}

bool equal(FunctionInfo* a, FunctionInfo* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (!equal(a->return_type, b->return_type)) return false;

    if (a->parameter_types.size() != b->parameter_types.size()) return false;
    for (int i = 0; i < a->parameter_types.size(); i++) {
        if (!equal(a->parameter_types[i], b->parameter_types[i]))return false;
    }
    return true;
}

bool equal(ClassInfo* a, ClassInfo* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (a->methods.size() != b->methods.size()) return false;
    for (auto it : a->methods) {
        if (b->methods.count(it.first) == 0)return false;
        if (!equal(it.second, b->methods[it.first])) return false;
    }
    if (a->fields.size() != b->fields.size()) return false;
    for (auto it : a->fields) {
        if (b->fields.count(it.first) == 0)return false;
        if (!equal(it.second, b->fields[it.first])) return false;
    }
    return true;
}


bool equal(SymbolInfo* a, SymbolInfo* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (a->type == b->type) {
        switch (a->type) {
            case SINFO::FUNCTION:
                return equal(a->function_info, b->function_info);
            case SINFO::CLASS:
                return equal(a->class_info, b->class_info);
            case SINFO::SIMPLE:
                return equal(a->simple_info, b->simple_info);
        }
    }
    return false;
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
    std::string text = "fun foo(){}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("foo");
    EXPECT_TRUE(equal(ginfo, w_finfo({}, nullptr)));
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
    SymbolInfo* ginfo = fp.globals->get("Foo");
    EXPECT_EQ(ginfo->type, SINFO::CLASS);
}

TEST(semantic_test, class_foo_eq) {
    std::string text = "class Foo{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("Foo");
    EXPECT_TRUE(equal(ginfo, w_cinfo(MapStringToSimple(), MapStringToFunction())));
}

TEST(semantic_test, class_foo_with_field) {
    std::string text = "class Foo{var x: String;}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("Foo");
    MapStringToSimple fields;
    fields["x"] = s_info(typenode_string);
    EXPECT_TRUE(equal(ginfo, w_cinfo(fields, MapStringToFunction())));
}

TEST(semantic_test, class_foo_with_method) {
    std::string text = "class Foo{fun foo(x:Integer)->String{}}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("Foo");
    MapStringToFunction methods;
    methods["foo"] = f_info({typenode_integer}, typenode_string);
    EXPECT_TRUE(equal(ginfo, w_cinfo(MapStringToSimple(), methods)));
}

TEST(semantic_test, class_foo_complete) {
    std::string text = "class Foo{var y: String; var z: Integer; fun foo(x:Integer)->String{} fun bar(w: List[String], t: Dict[String, Integer])->Integer{}}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo* ginfo = fp.globals->get("Foo");
    MapStringToFunction methods;
    methods["foo"] = f_info({typenode_integer}, typenode_string);
    methods["bar"] = f_info({typenode_list(typenode_string), typenode_dict(typenode_string, typenode_integer)},
                            typenode_integer);
    MapStringToSimple fields;
    fields["y"] = s_info(typenode_string);
    fields["z"] = s_info(typenode_integer);
    EXPECT_TRUE(equal(ginfo, w_cinfo(fields, methods)));
}
