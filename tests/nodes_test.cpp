#include "catch.hpp"
#include "../src/nodes/nodes.h"
#include "../src/nodes/ObjectType.h"
#include "../src/nodes/TypeclassNode.h"
#include "../src/nodes/InstanceNode.h"

const TextPosition DUMMY_POS = {0, 0};

TEST_CASE("nodes_number_integer", "[number]") {
    NumberNode n(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "number"},
                        {"number", {{"num_type", "integer"}, {"str", "7"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_number_float", "[number]") {
    NumberNode n(NumberType::FLOAT, "7.98", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "number"},
                        {"number", {{"num_type", "float"}, {"str", "7.98"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_number_double", "[number]") {
    NumberNode n(NumberType::DOUBLE, "7.98", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "number"},
                        {"number", {{"num_type", "double"}, {"str", "7.98"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_string", "[string]") {
    StringNode n("hello", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "string"},
                        {"string", {{"str", "hello"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_boolean", "[boolean]") {
    BooleanNode n(true, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",    "boolean"},
                        {"boolean", {{"value", true}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_id", "[id]") {
    IdNode n("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "id"},
                        {"id",   {{"_id", "foo"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_list", "[list]") {
    std::unique_ptr<Node> a = std::make_unique<NumberNode>(NumberType::INTEGER, "1", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> b = std::make_unique<NumberNode>(NumberType::INTEGER, "2", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> c = std::make_unique<NumberNode>(NumberType::INTEGER, "3", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "list"},
                        {"list", {{"elements", nlohmann::json::array({a->to_json(), b->to_json(), c->to_json()})}}}};
    VectorOfNodesU v;
    v.push_back(std::move(a));
    v.push_back(std::move(b));
    v.push_back(std::move(c));
    ListNode n(v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_call_no_args", "[call]") {
    IdNode* f = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    CallNode n(f, {}, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "call"},
                        {"call", {{"function", f->to_json()}, {"arguments", nlohmann::json::array()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_call", "[call]") {
    IdNode* f = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    IdNode* arg_0 = new IdNode("bar", DUMMY_POS, DUMMY_POS);
    NumberNode* arg_1 = new NumberNode(NumberType::INTEGER, "3", DUMMY_POS, DUMMY_POS);
    CallNode n(f, {arg_0, arg_1}, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "call"},
                        {"call", {{"function", f->to_json()}, {"arguments", {arg_0->to_json(), arg_1->to_json()}}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_assignment", "[assignment]") {
    std::unique_ptr<Node> l = std::make_unique<IdNode>("foo", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> r = std::make_unique<NumberNode>(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);

    nlohmann::json e = {{"type",       "assignment"},
                        {"assignment", {{"lvalue", l->to_json()}, {"rvalue", r->to_json()}}}};

    AssignmentNode n(l, r, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_declaration_no_type", "[declaration]") {
    std::unique_ptr<Node> r = std::make_unique<NumberNode>(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", {}}, {"expression", r->to_json()}}}};
    DeclarationNode n("foo", nullptr, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_declaration_with_type", "[declaration]") {
    std::unique_ptr<Node> r = std::make_unique<NumberNode>(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    ObjectType* t = new ObjectType("Integer");
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", t->to_json()}, {"expression", r->to_json()}}}};
    DeclarationNode n("foo", t, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_no_else", "[if]") {
    std::unique_ptr<Node> r = std::make_unique<NumberNode>(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    ObjectType* t = new ObjectType("Integer");
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", t->to_json()}, {"expression", r->to_json()}}}};
    DeclarationNode n("foo", t, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_with_else", "[if]") {
    std::unique_ptr<Node> c = std::make_unique<IdNode>("foo", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> n1 = std::make_unique<NumberNode>(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<DeclarationNode> d = std::make_unique<DeclarationNode>("foo",
                                                                           nullptr,
                                                                           n1,
                                                                           DUMMY_POS,
                                                                           DUMMY_POS,
                                                                           DUMMY_POS);
    VectorOfNodesU vector;
    vector.push_back(std::move(d));
    std::unique_ptr<BlockNode> t = std::make_unique<BlockNode>(std::move(vector), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v;
    std::unique_ptr<Node> n2 = std::make_unique<NumberNode>(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v.push_back(std::make_unique<DeclarationNode>("bar", nullptr, n2, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<BlockNode> l = std::make_unique<BlockNode>(std::move(v), DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "if"}};
    e["if"]["condition"] = c->to_json();
    e["if"]["then"] = t->to_json();
    e["if"]["elifs"] = nlohmann::json::array();
    e["if"]["else"] = l->to_json();
    IfNode n(c, t, {}, l, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_with_elif", "[if]") {
    std::unique_ptr<Node> c = std::make_unique<IdNode>("foo", DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v;
    std::unique_ptr<Node> n1 = std::make_unique<NumberNode>(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    v.push_back(std::make_unique<DeclarationNode>("foo", nullptr, n1, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<BlockNode> t = std::make_unique<BlockNode>(std::move(v), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v2;
    std::unique_ptr<Node> n2 = std::make_unique<NumberNode>(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v2.push_back(std::make_unique<DeclarationNode>("bar", nullptr, n2, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<BlockNode> l = std::make_unique<BlockNode>(std::move(v2), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v3;
    std::unique_ptr<Node> n3 = std::make_unique<NumberNode>(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v3.push_back(std::make_unique<DeclarationNode>("bar", nullptr, n3, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    BlockNode* elif_body_0 = new BlockNode(std::move(v3), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v4;
    std::unique_ptr<Node> n4 = std::make_unique<NumberNode>(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    v4.push_back(std::make_unique<DeclarationNode>("foo", nullptr, n4, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    BlockNode* elif_body_1 = new BlockNode(std::move(v4), DUMMY_POS, DUMMY_POS);

    IdNode* elif_cond_0 = new IdNode("a", DUMMY_POS, DUMMY_POS);
    IdNode* elif_cond_1 = new IdNode("b", DUMMY_POS, DUMMY_POS);

    nlohmann::json e = {{"type", "if"}};
    e["if"]["condition"] = c->to_json();
    e["if"]["elifs"] = nlohmann::json::array({{{"condition", elif_cond_0->to_json()}, {"then", elif_body_0->to_json()}},
                                              {{"condition", elif_cond_1->to_json()}, {"then", elif_body_1->to_json()}}});
    e["if"]["then"] = t->to_json();
    e["if"]["else"] = l->to_json();

    IfNode n(c,
             t,
             {{elif_cond_0, elif_body_0},
              {elif_cond_1, elif_body_1}},
             l,
             DUMMY_POS,
             DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_for", "[for]") {
    VectorOfNodesU v;
    std::unique_ptr<BlockNode> b = std::make_unique<BlockNode>(std::move(v), DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> exp = std::make_unique<IdNode>("bar", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "for"},
                        {"for",  {{"var", "foo"}, {"exp", exp->to_json()}, {"body", b->to_json()}}}};
    ForNode n("foo", exp, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_none", "[none]") {
    NoneNode n(DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "none"}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_break", "[break]") {
    BreakNode n(DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "break"}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_continue", "[continue]") {
    ContinueNode n(DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "continue"}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_return_no_value", "[return]") {
    ReturnNode n(nullptr, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "return"},
                        {"return", {{"expression", {}}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_return_with_value", "[return]") {
    IdNode* exp = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    ReturnNode n(exp, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "return"},
                        {"return", {{"expression", exp->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_subscript", "[subscript]") {
    IdNode* i = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    NumberNode* s = new NumberNode(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    SubscriptNode n(i, {s}, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",      "subscript"},
                        {"subscript", {{"parent", i->to_json()}, {"child", s->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_binop", "[binop]") {
    std::unique_ptr<Node> a = std::make_unique<IdNode>("foo", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> b = std::make_unique<NumberNode>(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",  "binop"},
                        {"binop", {{"left", a->to_json()}, {"right", b->to_json()}, {"op", op_to_string(OpType::ADD)}}}};
    BinopNode n(OpType::ADD, a, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_empty_list", "[empty_list]") {
    ObjectType* t = new ObjectType("Integer", {});
    EmptyListNode n(t, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",       "empty_list"},
                        {"empty_list", {{"type", t->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_empty_dict", "[empty_dict]") {
    ObjectType* k = new ObjectType("String", {});
    ObjectType* v = new ObjectType("Integer", {});
    EmptyDictNode n(k, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",       "empty_dict"},
                        {"empty_dict", {{"key_type", k->to_json()}, {"value_type", v->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_dict", "[dict]") {
    std::unique_ptr<Node> k1 = std::make_unique<StringNode>("foo", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> v1 = std::make_unique<NumberNode>(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> k2 = std::make_unique<StringNode>("bar", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> v2 = std::make_unique<NumberNode>(NumberType::INTEGER, "11", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "dict"},
                        {"dict", {{"items", {{{"key", k1->to_json()}, {"value", v1->to_json()}}, {{"key", k2->to_json()}, {"value", v2->to_json()}}}}}}};
    std::vector<std::pair<std::unique_ptr<Node>, std::unique_ptr<Node>>> d;
    d.emplace_back(std::move(k1), std::move(v1));
    d.emplace_back(std::move(k2), std::move(v2));
    DictNode n(d, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_member", "[member]") {
    std::unique_ptr<Node> p = std::make_unique<IdNode>("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",   "member"},
                        {"member", {{"parent", p->to_json()}, {"child", "bar"}}}};
    MemberNode n(p, Token(TokType::ID, "bar", DUMMY_POS));
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_function_no_args", "[function]") {
    BlockNode* b = new BlockNode({}, DUMMY_POS, DUMMY_POS);
    ObjectType* rt = new ObjectType("Integer", {});
    FunctionNode n("foo", {}, {}, rt, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",     "function"},
                        {"function", {{"id", "foo"}, {"parameters", nlohmann::json::array()}, {"body", b->to_json()}, {"return_type", rt->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_function_args", "[function]") {
    BlockNode* body = new BlockNode({}, DUMMY_POS, DUMMY_POS);
    ObjectType* a = new ObjectType("Integer", {});
    ObjectType* b = new ObjectType("String", {});
    ObjectType* rt = new ObjectType("Integer", {});
    FunctionNode n("foo", {"bar", "baz"}, {a, b}, rt, body, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",     "function"},
                        {"function", {{"id", "foo"}, {"parameters", {{{"id", "bar"}, {"type", a->to_json()}}, {{"id", "baz"}, {"type", b->to_json()}}}}, {"body", body->to_json()}, {"return_type", rt->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_while", "[while]") {
    BlockNode* body = new BlockNode({}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> a = std::make_unique<IdNode>("bar", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> b = std::make_unique<IdNode>("baz", DUMMY_POS, DUMMY_POS);
    BoolOpNode* cond = new BoolOpNode(BoolOp::EQ, a, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",  "while"},
                        {"while", {{"condition", cond->to_json()}, {"body", body->to_json()}}}};
    WhileNode n(cond, body, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_class_empty", "[class]") {
    ClassNode n("MyClass", {}, {}, {}, {}, {}, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json members = nlohmann::json::array();
    nlohmann::json methods;
    nlohmann::json static_methods;
    nlohmann::json e = {{"type",  "class"},
                        {"class", {{"id", "MyClass"}, {"members", members}, {"methods", methods}, {"static_methods", static_methods}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_class_full", "[class]") {
    ObjectType* t1 = new ObjectType("Integer");
    ObjectType* t2 = new ObjectType("String");
    VectorOfStrings members_ordered = {"foo", "bar"};
    ObjectType* rt = new ObjectType("Integer");
    BlockNode* b = new BlockNode({}, DUMMY_POS, DUMMY_POS);
    FunctionNode* method1 = new FunctionNode("method1", {}, {}, rt, b, DUMMY_POS, DUMMY_POS);
    FunctionNode* method2 = new FunctionNode("method2", {}, {}, rt, b, DUMMY_POS, DUMMY_POS);
    std::unordered_map<std::string, Method> cmethods = {{"method1", {nullptr, method1}},
                                                        {"method2", {nullptr, method2}}};
    std::vector<std::pair<std::string, TypeNode*>> cmembers = {{"foo", t1},
                                                               {"bar", t2}};


    nlohmann::json members = {{{"id", "foo"}, {"type", t1->to_json()}},
                              {{"id", "bar"}, {"type", t2->to_json()}}};
    nlohmann::json methods = {{"method2", method2->to_json()},
                              {"method1", method1->to_json()}};

    FunctionNode* smethod1 = new FunctionNode("smethod1", {}, {}, rt, b, DUMMY_POS, DUMMY_POS);
    FunctionNode* smethod2 = new FunctionNode("smethod2", {}, {}, rt, b, DUMMY_POS, DUMMY_POS);
    std::unordered_map<std::string, FunctionNode*> cstatic_methods = {{"smethod1", smethod1},
                                                                      {"smethod2", smethod2}};
    ClassNode n("MyClass", {"k", "v"}, cmembers, cmethods, {}, cstatic_methods, DUMMY_POS, DUMMY_POS);
    n.members_ordered = members_ordered;
    nlohmann::json nj = n.to_json();

    nlohmann::json static_methods = {{"smethod1", smethod1->to_json()},
                                     {"smethod2", smethod2->to_json()}};

    nlohmann::json e = {{"type",  "class"},
                        {"class", {{"id", "MyClass"}, {"members", members}, {"methods", methods}, {"static_methods", static_methods}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_typeclass", "[typeclass]") {
    FunctionType* method1 = new FunctionType({new ObjectType("t"), new ObjectType("t")}, new ObjectType("Boolean"));
    FunctionType* method2 = new FunctionType({new ObjectType("t"), new ObjectType("t")}, new ObjectType("Boolean"));
    std::unordered_map<std::string, FunctionType*> cmethods = {{"method1", method1},
                                                               {"method2", method2}};

    TypeclassNode n("Comparable", "t", cmethods, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    nlohmann::json e = {{"type",      "typeclass"},
                        {"typeclass", {{"id", "Comparable"}, {"base_type", "t"}, {"methods", {{"method1", method1->to_json()}, {"method2", method2->to_json()}}}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_instance", "[instance]") {
    // FunctionType* method1 = new FunctionType({new ObjectType("a"), new ObjectType("b")}, new ObjectType("Boolean"));
    // FunctionType* method2 = new FunctionType({new ObjectType("c"), new ObjectType("c")}, new ObjectType("Boolean"));
    ObjectType* rt = new ObjectType("Integer");
    BlockNode* b = new BlockNode({}, DUMMY_POS, DUMMY_POS);
    FunctionNode* method1 = new FunctionNode("method1", {}, {}, rt, b, DUMMY_POS, DUMMY_POS);
    FunctionNode* method2 = new FunctionNode("method2", {}, {}, rt, b, DUMMY_POS, DUMMY_POS);

    std::unordered_map<std::string, FunctionNode*> cmethods = {{"method1", method1},
                                                               {"method2", method2}};

    ObjectType* bt = new ObjectType("SomeType");
    InstanceNode n("Comparable", bt, cmethods, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    nlohmann::json e = {{"type",     "instance"},
                        {"instance", {{"id", "Comparable"}, {"base_type", bt->to_json()}, {"methods", {{"method1", method1->to_json()}, {"method2", method2->to_json()}}}}}};
    REQUIRE(e == nj);
}