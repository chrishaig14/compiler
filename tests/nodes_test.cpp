#include "catch.hpp"
#include "../src/ast/nodes.h"
#include "../src/ast/ObjectType.h"
#include "../src/ast/TypeclassNode.h"
#include "../src/ast/InstanceNode.h"

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
    ast::IdNode n("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "id"},
                        {"id",   {{"_id", "foo"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_list", "[list]") {
    UNode a = NumberNode::make(NumberType::INTEGER, "1", DUMMY_POS, DUMMY_POS);
    UNode b = NumberNode::make(NumberType::INTEGER, "2", DUMMY_POS, DUMMY_POS);
    UNode c = NumberNode::make(NumberType::INTEGER, "3", DUMMY_POS, DUMMY_POS);
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
    UNode f = ast::IdNode::make("foo", DUMMY_POS, DUMMY_POS);
    VectorOfNodesU vector;
    nlohmann::json e = {{"type", "call"},
                        {"call", {{"function", f->to_json()}, {"arguments", nlohmann::json::array()}}}};
    CallNode n(f, vector, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_call", "[call]") {
    UNode f = ast::IdNode::make("foo", DUMMY_POS, DUMMY_POS);
    UNode arg_0 = ast::IdNode::make("bar", DUMMY_POS, DUMMY_POS);
    UNode arg_1 = NumberNode::make(NumberType::INTEGER, "3", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "call"},
                        {"call", {{"function", f->to_json()}, {"arguments", {arg_0->to_json(), arg_1->to_json()}}}}};
    VectorOfNodesU v;
    v.push_back(std::move(arg_0));
    v.push_back(std::move(arg_1));
    CallNode n(f, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_assignment", "[assignment]") {
    UNode l = ast::IdNode::make("foo", DUMMY_POS, DUMMY_POS);
    UNode r = NumberNode::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);

    nlohmann::json e = {{"type",       "assignment"},
                        {"assignment", {{"lvalue", l->to_json()}, {"rvalue", r->to_json()}}}};

    AssignmentNode n(l, r, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_declaration_no_type", "[declaration]") {
    UNode r = NumberNode::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", {}}, {"expression", r->to_json()}}}};
    ast::DeclarationNode n("foo", nullptr, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_declaration_with_type", "[declaration]") {
    UNode r = NumberNode::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    ObjectType* t = new ObjectType("Integer");
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", t->to_json()}, {"expression", r->to_json()}}}};
    ast::DeclarationNode n("foo", t, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_no_else", "[if]") {
    UNode r = NumberNode::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    ObjectType* t = new ObjectType("Integer");
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", t->to_json()}, {"expression", r->to_json()}}}};
    ast::DeclarationNode n("foo", t, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_with_else", "[if]") {
    UNode c = ast::IdNode::make("foo", DUMMY_POS, DUMMY_POS);
    UNode n1 = NumberNode::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::DeclarationNode> d = std::make_unique<ast::DeclarationNode>("foo",
                                                                           nullptr,
                                                                           n1,
                                                                           DUMMY_POS,
                                                                           DUMMY_POS,
                                                                           DUMMY_POS);
    VectorOfNodesU vector;
    vector.push_back(std::move(d));
    std::unique_ptr<BlockNode> t = BlockNode::make(std::move(vector), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v;
    UNode n2 = NumberNode::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v.push_back(std::make_unique<ast::DeclarationNode>("bar", nullptr, n2, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<BlockNode> l = BlockNode::make(std::move(v), DUMMY_POS, DUMMY_POS);
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
    UNode c = ast::IdNode::make("foo", DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v;
    UNode n1 = NumberNode::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    v.push_back(std::make_unique<ast::DeclarationNode>("foo", nullptr, n1, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<BlockNode> t = BlockNode::make(std::move(v), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v2;
    UNode n2 = NumberNode::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v2.push_back(std::make_unique<ast::DeclarationNode>("bar", nullptr, n2, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<BlockNode> l = BlockNode::make(std::move(v2), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v3;
    UNode n3 = NumberNode::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v3.push_back(std::make_unique<ast::DeclarationNode>("bar", nullptr, n3, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    BlockNode* elif_body_0 = new BlockNode(std::move(v3), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v4;
    UNode n4 = NumberNode::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    v4.push_back(std::make_unique<ast::DeclarationNode>("foo", nullptr, n4, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    BlockNode* elif_body_1 = new BlockNode(std::move(v4), DUMMY_POS, DUMMY_POS);

    ast::IdNode* elif_cond_0 = new ast::IdNode("a", DUMMY_POS, DUMMY_POS);
    ast::IdNode* elif_cond_1 = new ast::IdNode("b", DUMMY_POS, DUMMY_POS);

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
    std::unique_ptr<BlockNode> b = BlockNode::make(std::move(v), DUMMY_POS, DUMMY_POS);
    UNode exp = ast::IdNode::make("bar", DUMMY_POS, DUMMY_POS);
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
    UNode ptr;
    ReturnNode n(ptr, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "return"},
                        {"return", {{"expression", {}}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_return_with_value", "[return]") {
    UNode exp = ast::IdNode::make("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",   "return"},
                        {"return", {{"expression", exp->to_json()}}}};
    ReturnNode n(exp, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_subscript", "[subscript]") {
    UNode i = ast::IdNode::make("foo", DUMMY_POS, DUMMY_POS);
    UNode s = NumberNode::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",      "subscript"},
                        {"subscript", {{"parent", i->to_json()}, {"child", s->to_json()}}}};
    VectorOfNodesU v;
    v.push_back(std::move(s));
    SubscriptNode n(i, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_binop", "[binop]") {
    UNode a = ast::IdNode::make("foo", DUMMY_POS, DUMMY_POS);
    UNode b = NumberNode::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",  "binop"},
                        {"binop", {{"left", a->to_json()}, {"right", b->to_json()}, {"op", op_to_string(OpType::ADD)}}}};
    ast::BinopNode n(OpType::ADD, a, b, DUMMY_POS, DUMMY_POS);
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
    UTypeNode k = std::make_unique<ObjectType>("String");
    UTypeNode v = std::make_unique<ObjectType>("Integer");
    nlohmann::json e = {{"type",       "empty_dict"},
                        {"empty_dict", {{"key_type", k->to_json()}, {"value_type", v->to_json()}}}};
    EmptyDictNode n(k, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_dict", "[dict]") {
    UNode k1 = std::make_unique<StringNode>("foo", DUMMY_POS, DUMMY_POS);
    UNode v1 = NumberNode::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    UNode k2 = std::make_unique<StringNode>("bar", DUMMY_POS, DUMMY_POS);
    UNode v2 = NumberNode::make(NumberType::INTEGER, "11", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "dict"},
                        {"dict", {{"items", {{{"key", k1->to_json()}, {"value", v1->to_json()}}, {{"key", k2->to_json()}, {"value", v2->to_json()}}}}}}};
    std::vector<std::pair<UNode, UNode>> d;
    d.emplace_back(std::move(k1), std::move(v1));
    d.emplace_back(std::move(k2), std::move(v2));
    DictNode n(d, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_member", "[member]") {
    UNode p = ast::IdNode::make("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",   "member"},
                        {"member", {{"parent", p->to_json()}, {"child", "bar"}}}};
    MemberNode n(p, Token(TokType::ID, "bar", DUMMY_POS));
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_function_no_args", "[function]") {
    VectorOfNodesU v;
    std::unique_ptr<BlockNode> b = BlockNode::make(std::move(v), DUMMY_POS, DUMMY_POS);
    UTypeNode rt = std::make_unique<ObjectType>("Integer");
    nlohmann::json e = {{"type",     "function"},
                        {"function", {{"id", "foo"}, {"parameters", nlohmann::json::array()}, {"body", b->to_json()}, {"return_type", rt->to_json()}}}};
    VectorOfUTypes vt;
    FunctionNode n("foo", {}, vt, rt, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_function_args", "[function]") {
    std::unique_ptr<BlockNode> body = BlockNode::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    UTypeNode a = std::make_unique<ObjectType>("Integer", VectorOfTypes{});
    UTypeNode b = std::make_unique<ObjectType>("String", VectorOfTypes{});
    UTypeNode rt = std::make_unique<ObjectType>("Integer");
    nlohmann::json e = {{"type",     "function"},
                        {"function", {{"id", "foo"}, {"parameters", {{{"id", "bar"}, {"type", a->to_json()}}, {{"id", "baz"}, {"type", b->to_json()}}}}, {"body", body->to_json()}, {"return_type", rt->to_json()}}}};
    VectorOfUTypes vt;
    vt.push_back(std::move(a));
    vt.push_back(std::move(b));
    FunctionNode n("foo", {"bar", "baz"}, vt, rt, body, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_while", "[while]") {
    std::unique_ptr<BlockNode> body = BlockNode::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    UNode a = ast::IdNode::make("bar", DUMMY_POS, DUMMY_POS);
    UNode b = ast::IdNode::make("baz", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<Node> cond = std::make_unique<BoolOpNode>(BoolOp::EQ, a, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",  "while"},
                        {"while", {{"condition", cond->to_json()}, {"body", body->to_json()}}}};
    WhileNode n(cond, body, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_class_empty", "[class]") {
    std::unordered_map<std::string, UFunctionNode> v;
    ClassNode n("MyClass", {}, {}, {}, {}, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json members = nlohmann::json::array();
    nlohmann::json methods;
    nlohmann::json static_methods;
    nlohmann::json e = {{"type",  "class"},
                        {"class", {{"id", "MyClass"}, {"members", members}, {"methods", methods}, {"static_methods", static_methods}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_class_full", "[class]") {
    auto t1 = std::make_unique<ObjectType>("Integer");
    auto t2 = std::make_unique<ObjectType>("String");
    VectorOfStrings members_ordered = {"foo", "bar"};
    UTypeNode rt1 = std::make_unique<ObjectType>("Integer");
    UTypeNode rt2 = std::make_unique<ObjectType>("Integer");
    UTypeNode rt3 = std::make_unique<ObjectType>("Integer");
    UTypeNode rt4 = std::make_unique<ObjectType>("Integer");
    std::unique_ptr<BlockNode> b1 = BlockNode::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<BlockNode> b2 = BlockNode::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<BlockNode> b3 = BlockNode::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<BlockNode> b4 = BlockNode::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    VectorOfUTypes v1;
    VectorOfUTypes v2;
    FunctionNode* method1 = new FunctionNode("method1", {}, v1, rt1, b1, DUMMY_POS, DUMMY_POS);
    FunctionNode* method2 = new FunctionNode("method2", {}, v2, rt2, b2, DUMMY_POS, DUMMY_POS);
    std::unordered_map<std::string, Method> cmethods = {{"method1", {nullptr, method1}},
                                                        {"method2", {nullptr, method2}}};
    nlohmann::json members = {{{"id", "foo"}, {"type", t1->to_json()}},
                              {{"id", "bar"}, {"type", t2->to_json()}}};
    std::vector<std::pair<std::string, UTypeNode>> cmembers;
    cmembers.emplace_back("foo", std::move(t1));
    cmembers.emplace_back("bar", std::move(t2));

    nlohmann::json methods = {{"method2", method2->to_json()},
                              {"method1", method1->to_json()}};

    VectorOfUTypes vv1, vv2;
    UFunctionNode smethod1 = std::make_unique<FunctionNode>("smethod1",
                                                            VectorOfStrings{},
                                                            vv1,
                                                            rt3,
                                                            b3,
                                                            DUMMY_POS,
                                                            DUMMY_POS);
    UFunctionNode smethod2 = std::make_unique<FunctionNode>("smethod2",
                                                            VectorOfStrings{},
                                                            vv2,
                                                            rt4,
                                                            b4,
                                                            DUMMY_POS,
                                                            DUMMY_POS);
    std::unordered_map<std::string, UFunctionNode> cstatic_methods;
    nlohmann::json static_methods = {{"smethod1", smethod1->to_json()},
                                     {"smethod2", smethod2->to_json()}};
    cstatic_methods["smethod1"] = std::move(smethod1);
    cstatic_methods["smethod2"] = std::move(smethod2);
    ClassNode n("MyClass", {"k", "v"}, std::move(cmembers), cmethods, {}, cstatic_methods, DUMMY_POS, DUMMY_POS);
    n.members_ordered = members_ordered;
    nlohmann::json nj = n.to_json();


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
    UTypeNode rt1 = std::make_unique<ObjectType>("Integer");
    UTypeNode rt2 = std::make_unique<ObjectType>("Integer");
    std::unique_ptr<BlockNode> b1 = BlockNode::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<BlockNode> b2 = BlockNode::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    VectorOfUTypes v1;
    VectorOfUTypes v2;
    FunctionNode* method1 = new FunctionNode("method1", {}, v1, rt1, b1, DUMMY_POS, DUMMY_POS);
    FunctionNode* method2 = new FunctionNode("method2", {}, v2, rt2, b2, DUMMY_POS, DUMMY_POS);

    std::unordered_map<std::string, FunctionNode*> cmethods = {{"method1", method1},
                                                               {"method2", method2}};

    ObjectType* bt = new ObjectType("SomeType");
    InstanceNode n("Comparable", bt, cmethods, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    nlohmann::json e = {{"type",     "instance"},
                        {"instance", {{"id", "Comparable"}, {"base_type", bt->to_json()}, {"methods", {{"method1", method1->to_json()}, {"method2", method2->to_json()}}}}}};
    REQUIRE(e == nj);
}