#include <gtest/gtest.h>
#include <Scanner.h>
#include <Parser.h>
#include <FirstPass.h>
#include <SecondPass.h>

void clean(FunctionNode node);

void clean(SubscriptNode node);

void clean(CallNode node);

void clean(AssignmentNode node);

void clean(BinopNode node);

void clean(DeclarationNode node);

void clean(MemberNode node);

void clean(IfNode node);

void clean(ListNode pNode);

void clean(ClassNode node);

void clean(ReturnNode node);

void clean(TypeNode node) {}


void clean(VectorOfNodes v) {
    for (auto n: v) clean(n);
}

void clean(AstNode& node) {
    switch (node.type) {
        case AstType::RETURN:
            clean(*node.ast_return);
            delete node.ast_return;
            break;
        case AstType::LIST:
            clean(*node.ast_list);
            delete node.ast_list;
            break;
        case AstType::CLASS:
            clean(*node.ast_class);
            delete node.ast_class;
            break;
        case AstType::IF:
            clean(*node.ast_if);
            delete node.ast_if;
            break;
        case AstType::FUNCTION:
            clean(*node.ast_function);
            delete node.ast_function;
            break;
        case AstType::MEMBER:
            clean(*node.ast_member);
            delete node.ast_member;
            break;
        case AstType::ASSIGNMENT:
            clean(*node.ast_assignment);
            delete node.ast_assignment;
            break;
        case AstType::BINOP:
            clean(*node.ast_binop);
            delete node.ast_binop;
            break;
        case AstType::IDENTIFIER:
            delete node.ast_identifier;
            break;
        case AstType::DECLARATION:
            clean(*node.ast_declaration);
            delete node.ast_declaration;
            break;
        case AstType::TYPE:
            delete node.ast_type;
            break;
        case AstType::NUMBER:
            delete node.ast_number;
            break;
        case AstType::CALL:
            clean(*node.ast_call);
            delete node.ast_call;
            break;
        case AstType::SUB:
            clean(*node.ast_sub);
            delete node.ast_sub;
            break;
        case AstType::STRING:
            delete node.ast_string;
            break;
    }
}

void clean(FunctionNode node) {
    clean(node.body);
}

void clean(SubscriptNode node) {
    clean(node.parent);
    clean(node.sub);
}

void clean(CallNode node) {
    clean(node.function);
    for (auto a: node.arguments) clean(a);
}

void clean(AssignmentNode node) {
    clean(node.lvalue);
    clean(node.rvalue);
}

void clean(ClassNode node) {
    for (auto f: node.fields)clean(f);
    for (auto m: node.methods)clean(m);
}

void clean(BinopNode node) {
    clean(node.left);
    clean(node.right);
}

void clean(DeclarationNode node) {
    if (node.expression != nullptr) {
        clean(*node.expression);
        delete node.expression;
    }
    if (node.type != nullptr) {
        clean(*node.type);
        delete node.type;
    }
}

void clean(MemberNode node) {
    clean(node.parent);
}

void clean(IfNode node) {
    clean(node.condition);
    clean(node.then);
}

void clean(ListNode pNode) {
    for (auto e:pNode.elements)clean(e);
}

void clean(ReturnNode node) {
    clean(node.expression);
}

void assert_eq_si(ObjectInfo t1, ObjectInfo t2) {
    EXPECT_EQ(t1.parent, t2.parent);
    EXPECT_EQ(t1.type_parameters.size(), t2.type_parameters.size());
    for (int i = 0; i < t1.type_parameters.size(); i++) {
        assert_eq_si(t1.type_parameters[i], t2.type_parameters[i]);
    }
}

VectorOfNodes get_tree(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    return parser.parse_program();
}

TEST(first_pass_test, fun_foo) {
    std::string text = "fun foo()->None{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo ginfo = fp.globals.get("foo");
    EXPECT_EQ(ginfo.type, SINFO::FUNCTION);
    clean(tree);
}

TypeNode typenode_integer = i_type("Integer", {});
TypeNode typenode_string = i_type("String", {});
TypeNode typenode_boolean = i_type("Boolean", {});

TypeNode typenode_list(TypeNode el) {
    return i_type("List", {el});
}

TypeNode typenode_dict(TypeNode k, TypeNode v) {
    return i_type("Dict", {k, v});
}

TEST(first_pass_test, fun_foo_eq) {
    std::string text = "fun foo()->String{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo ginfo = fp.globals.get("foo");
    EXPECT_TRUE(equal(ginfo, w_finfo({}, typenode_string)));
    clean(tree);
}

TEST(first_pass_test, fun_foo_complete) {
    std::string text = "fun foo(y: Integer, x: String)-> Boolean{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    SymbolInfo ginfo = fp.globals.get("foo");
    EXPECT_TRUE(equal(ginfo, w_finfo({typenode_integer, typenode_string}, typenode_boolean)));
    clean(tree);
}

TEST(first_pass_test, class_foo) {
    std::string text = "class Foo{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    ClassInfo ginfo = fp.class_table.get("Foo");
    clean(tree);
}

TEST(first_pass_test, class_foo_eq) {
    std::string text = "class Foo{}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    clean(tree);
}

TEST(first_pass_test, class_foo_with_field) {
    std::string text = "class Foo{var x: String;}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    ClassInfo ginfo = fp.class_table.get("Foo");
    MapStringToSimple fields;
    fields["x"] = s_info(typenode_string);
    ClassInfo class_info;
    class_info.fields = fields;
    EXPECT_TRUE(equal(ginfo, class_info));
    clean(tree);
}

TEST(first_pass_test, class_foo_with_method) {
    std::string text = "class Foo{fun foo(x:Integer)->String{}}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    ClassInfo ginfo = fp.class_table.get("Foo");
    MapStringToFunction methods;
    methods["foo"] = f_info({typenode_integer}, typenode_string);
    ClassInfo expected;
    expected.methods = methods;
    EXPECT_TRUE(equal(ginfo, expected));
    clean(tree);
}

TEST(first_pass_test, class_foo_complete) {
    std::string text = "class Foo{var y: String; var z: Integer; fun foo(x:Integer)->String{} fun bar(w: List[String], t: Dict[String, Integer])->Integer{}}";
    VectorOfNodes tree = get_tree(text);
    FirstPass fp;
    fp.analyze(tree);
    ClassInfo ginfo = fp.class_table.get("Foo");
    MapStringToFunction methods;
    methods["foo"] = f_info({typenode_integer}, typenode_string);
    methods["bar"] = f_info({typenode_list(typenode_string), typenode_dict(typenode_string, typenode_integer)},
                            typenode_integer);
    MapStringToSimple fields;
    fields["y"] = s_info(typenode_string);
    fields["z"] = s_info(typenode_integer);
    ClassInfo expected;
    expected.fields = fields;
    expected.methods = methods;
    EXPECT_TRUE(equal(ginfo, expected));
    clean(tree);
}
