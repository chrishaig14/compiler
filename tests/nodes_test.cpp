#include "catch.hpp"
#include "../src/ast/nodes.h"
#include "../src/ast/ObjectType.h"
#include "../src/ast/Typeclass.h"
#include "../src/ast/Instance.h"

const TextPosition DUMMY_POS = {0, 0};

TEST_CASE("nodes_number_integer", "[number]") {
    ast::Number n(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "number"},
                        {"number", {{"num_type", "integer"}, {"str", "7"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_number_float", "[number]") {
    ast::Number n(NumberType::FLOAT, "7.98", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "number"},
                        {"number", {{"num_type", "float"}, {"str", "7.98"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_number_double", "[number]") {
    ast::Number n(NumberType::DOUBLE, "7.98", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "number"},
                        {"number", {{"num_type", "double"}, {"str", "7.98"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_string", "[string]") {
    ast::String n("hello", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "string"},
                        {"string", {{"str", "hello"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_boolean", "[boolean]") {
    ast::Boolean n(true, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",    "boolean"},
                        {"boolean", {{"value", true}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_id", "[id]") {
    ast::Id n("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "id"},
                        {"id",   {{"_id", "foo"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_list", "[list]") {
    UNode a = ast::Number::make(NumberType::INTEGER, "1", DUMMY_POS, DUMMY_POS);
    UNode b = ast::Number::make(NumberType::INTEGER, "2", DUMMY_POS, DUMMY_POS);
    UNode c = ast::Number::make(NumberType::INTEGER, "3", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "list"},
                        {"list", {{"elements", nlohmann::json::array({a->to_json(), b->to_json(), c->to_json()})}}}};
    VectorOfNodesU v;
    v.push_back(std::move(a));
    v.push_back(std::move(b));
    v.push_back(std::move(c));
    ast::List n(v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_call_no_args", "[call]") {
    UNode f = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    VectorOfNodesU vector;
    nlohmann::json e = {{"type", "call"},
                        {"call", {{"function", f->to_json()}, {"arguments", nlohmann::json::array()}}}};
    ast::Call n(f, vector, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_call", "[call]") {
    UNode f = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    UNode arg_0 = ast::Id::make("bar", DUMMY_POS, DUMMY_POS);
    UNode arg_1 = ast::Number::make(NumberType::INTEGER, "3", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "call"},
                        {"call", {{"function", f->to_json()}, {"arguments", {arg_0->to_json(), arg_1->to_json()}}}}};
    VectorOfNodesU v;
    v.push_back(std::move(arg_0));
    v.push_back(std::move(arg_1));
    ast::Call n(f, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_assignment", "[assignment]") {
    UNode l = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    UNode r = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);

    nlohmann::json e = {{"type",       "assignment"},
                        {"assignment", {{"lvalue", l->to_json()}, {"rvalue", r->to_json()}}}};

    ast::Assignment n(l, r, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_declaration_no_type", "[declaration]") {
    UNode r = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", {}}, {"expression", r->to_json()}}}};
    ast::Declaration n("foo", nullptr, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_declaration_with_type", "[declaration]") {
    UNode r = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    ObjectType to("Integer");
    auto* t = &to;
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", t->to_json()}, {"expression", r->to_json()}}}};
    ast::Declaration n("foo", t, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_no_else", "[if]") {
    UNode r = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    ObjectType to("Integer");
    auto* t = &to;
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", t->to_json()}, {"expression", r->to_json()}}}};
    ast::Declaration n("foo", t, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_with_else", "[if]") {
    UNode c = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    UNode n1 = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Declaration> d = std::make_unique<ast::Declaration>("foo",
                                                                             nullptr,
                                                                             n1,
                                                                             DUMMY_POS,
                                                                             DUMMY_POS,
                                                                             DUMMY_POS);
    VectorOfNodesU vector;
    vector.push_back(std::move(d));
    std::unique_ptr<ast::Block> t = ast::Block::make(std::move(vector), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v;
    UNode n2 = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v.push_back(std::make_unique<ast::Declaration>("bar", nullptr, n2, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<ast::Block> l = ast::Block::make(std::move(v), DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "if"}};
    e["if"]["condition"] = c->to_json();
    e["if"]["then"] = t->to_json();
    e["if"]["elifs"] = nlohmann::json::array();
    e["if"]["else"] = l->to_json();
    ast::If n(c, t, {}, l, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_with_elif", "[if]") {
    UNode c = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v;
    UNode n1 = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    v.push_back(std::make_unique<ast::Declaration>("foo", nullptr, n1, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<ast::Block> t = ast::Block::make(std::move(v), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v2;
    UNode n2 = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v2.push_back(std::make_unique<ast::Declaration>("bar", nullptr, n2, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<ast::Block> l = ast::Block::make(std::move(v2), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v3;
    UNode n3 = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v3.push_back(std::make_unique<ast::Declaration>("bar", nullptr, n3, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    auto elif_body_0 = ast::Block::make(std::move(v3), DUMMY_POS, DUMMY_POS);
    VectorOfNodesU v4;
    UNode n4 = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    v4.push_back(std::make_unique<ast::Declaration>("foo", nullptr, n4, DUMMY_POS, DUMMY_POS, DUMMY_POS));
    auto elif_body_1 = ast::Block::make(std::move(v4), DUMMY_POS, DUMMY_POS);

    auto elif_cond_0 = ast::Id::make("a", DUMMY_POS, DUMMY_POS);
    auto elif_cond_1 = ast::Id::make("b", DUMMY_POS, DUMMY_POS);

    nlohmann::json e = {{"type", "if"}};
    e["if"]["condition"] = c->to_json();
    e["if"]["elifs"] = nlohmann::json::array({{{"condition", elif_cond_0->to_json()}, {"then", elif_body_0->to_json()}},
                                              {{"condition", elif_cond_1->to_json()}, {"then", elif_body_1->to_json()}}});
    e["if"]["then"] = t->to_json();
    e["if"]["else"] = l->to_json();

    std::vector<std::pair<UNode, UBlockNode>> elifs;
    elifs.emplace_back(std::move(elif_cond_0), std::move(elif_body_0));
    elifs.emplace_back(std::move(elif_cond_1), std::move(elif_body_1));

    ast::If n(c, t, std::move(elifs), l, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_for", "[for]") {
    VectorOfNodesU v;
    std::unique_ptr<ast::Block> b = ast::Block::make(std::move(v), DUMMY_POS, DUMMY_POS);
    UNode exp = ast::Id::make("bar", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "for"},
                        {"for",  {{"var", "foo"}, {"exp", exp->to_json()}, {"body", b->to_json()}}}};
    ast::For n("foo", exp, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_none", "[none]") {
    ast::None n(DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "none"}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_break", "[break]") {
    ast::Break n(DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "break"}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_continue", "[continue]") {
    ast::Continue n(DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "continue"}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_return_no_value", "[return]") {
    UNode ptr;
    ast::Return n(ptr, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "return"},
                        {"return", {{"expression", {}}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_return_with_value", "[return]") {
    UNode exp = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",   "return"},
                        {"return", {{"expression", exp->to_json()}}}};
    ast::Return n(exp, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_subscript", "[subscript]") {
    UNode i = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    UNode s = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",      "subscript"},
                        {"subscript", {{"parent", i->to_json()}, {"child", s->to_json()}}}};
    VectorOfNodesU v;
    v.push_back(std::move(s));
    ast::Subscript n(i, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_binop", "[binop]") {
    UNode a = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    UNode b = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",  "binop"},
                        {"binop", {{"left", a->to_json()}, {"right", b->to_json()}, {"op", op_to_string(OpType::ADD)}}}};
    ast::BinaryOp n(OpType::ADD, a, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_empty_list", "[empty_list]") {
    ObjectType to("Integer", {});
    auto* t = &to;
    ast::EmptyList n(t, DUMMY_POS, DUMMY_POS);
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
    ast::EmptyDict n(k, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_dict", "[dict]") {
    UNode k1 = std::make_unique<ast::String>("foo", DUMMY_POS, DUMMY_POS);
    UNode v1 = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    UNode k2 = std::make_unique<ast::String>("bar", DUMMY_POS, DUMMY_POS);
    UNode v2 = ast::Number::make(NumberType::INTEGER, "11", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "dict"},
                        {"dict", {{"items", {{{"key", k1->to_json()}, {"value", v1->to_json()}}, {{"key", k2->to_json()}, {"value", v2->to_json()}}}}}}};
    std::vector<std::pair<UNode, UNode>> d;
    d.emplace_back(std::move(k1), std::move(v1));
    d.emplace_back(std::move(k2), std::move(v2));
    ast::DictNode n(d, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_member", "[member]") {
    UNode p = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",   "member"},
                        {"member", {{"parent", p->to_json()}, {"child", "bar"}}}};
    ast::Member n(p, Token(TokType::ID, "bar", DUMMY_POS));
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_function_no_args", "[function]") {
    VectorOfNodesU v;
    std::unique_ptr<ast::Block> b = ast::Block::make(std::move(v), DUMMY_POS, DUMMY_POS);
    UTypeNode rt = std::make_unique<ObjectType>("Integer");
    nlohmann::json e = {{"type",     "function"},
                        {"function", {{"id", "foo"}, {"parameters", nlohmann::json::array()}, {"body", b->to_json()}, {"return_type", rt->to_json()}}}};
    VectorOfUTypes vt;
    ast::Function n("foo", {}, vt, rt, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_function_args", "[function]") {
    std::unique_ptr<ast::Block> body = ast::Block::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    UTypeNode a = std::make_unique<ObjectType>("Integer", VectorOfTypes{});
    UTypeNode b = std::make_unique<ObjectType>("String", VectorOfTypes{});
    UTypeNode rt = std::make_unique<ObjectType>("Integer");
    nlohmann::json e = {{"type",     "function"},
                        {"function", {{"id", "foo"}, {"parameters", {{{"id", "bar"}, {"type", a->to_json()}}, {{"id", "baz"}, {"type", b->to_json()}}}}, {"body", body->to_json()}, {"return_type", rt->to_json()}}}};
    VectorOfUTypes vt;
    vt.push_back(std::move(a));
    vt.push_back(std::move(b));
    ast::Function n("foo", {"bar", "baz"}, vt, rt, body, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_while", "[while]") {
    std::unique_ptr<ast::Block> body = ast::Block::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    UNode a = ast::Id::make("bar", DUMMY_POS, DUMMY_POS);
    UNode b = ast::Id::make("baz", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Node> cond = std::make_unique<ast::BinaryOp>(OpType::EQ, a, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",  "while"},
                        {"while", {{"condition", cond->to_json()}, {"body", body->to_json()}}}};
    ast::While n(cond, body, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_class_empty", "[class]") {
    std::unordered_map<std::string, UFunctionNode> v;
    ast::Klass n("MyClass", {}, {}, {}, {}, v, DUMMY_POS, DUMMY_POS);
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
    std::unique_ptr<ast::Block> b1 = ast::Block::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Block> b2 = ast::Block::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Block> b3 = ast::Block::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Block> b4 = ast::Block::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    VectorOfUTypes v1;
    VectorOfUTypes v2;
    ast::Function* method1 = new ast::Function("method1", {}, v1, rt1, b1, DUMMY_POS, DUMMY_POS);
    ast::Function* method2 = new ast::Function("method2", {}, v2, rt2, b2, DUMMY_POS, DUMMY_POS);
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
    UFunctionNode smethod1 = std::make_unique<ast::Function>("smethod1",
                                                             VectorOfStrings{},
                                                             vv1,
                                                             rt3,
                                                             b3,
                                                             DUMMY_POS,
                                                             DUMMY_POS);
    UFunctionNode smethod2 = std::make_unique<ast::Function>("smethod2",
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
    ast::Klass n("MyClass", {"k", "v"}, std::move(cmembers), cmethods, {}, cstatic_methods, DUMMY_POS, DUMMY_POS);
    n.members_ordered = members_ordered;
    nlohmann::json nj = n.to_json();


    nlohmann::json e = {{"type",  "class"},
                        {"class", {{"id", "MyClass"}, {"members", members}, {"methods", methods}, {"static_methods", static_methods}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_typeclass", "[typeclass]") {
    FunctionType o_method1({new ObjectType("t"), new ObjectType("t")}, new ObjectType("Boolean"));
    FunctionType o_method2({new ObjectType("t"), new ObjectType("t")}, new ObjectType("Boolean"));
    auto* method1 = &o_method1;
    auto* method2 = &o_method2;
    std::unordered_map<std::string, FunctionType*> cmethods = {{"method1", method1},
                                                               {"method2", method2}};

    ast::Typeclass n("Comparable", "t", cmethods, DUMMY_POS, DUMMY_POS);
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
    std::unique_ptr<ast::Block> b1 = ast::Block::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Block> b2 = ast::Block::make(VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    VectorOfUTypes v1;
    VectorOfUTypes v2;
    ast::Function* method1 = new ast::Function("method1", {}, v1, rt1, b1, DUMMY_POS, DUMMY_POS);
    ast::Function* method2 = new ast::Function("method2", {}, v2, rt2, b2, DUMMY_POS, DUMMY_POS);

    std::unordered_map<std::string, ast::Function*> cmethods = {{"method1", method1},
                                                                {"method2", method2}};

    ObjectType bto("SomeType");
    auto* bt = &bto;
    ast::Instance n("Comparable", bt, cmethods, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    nlohmann::json e = {{"type",     "instance"},
                        {"instance", {{"id", "Comparable"}, {"base_type", bt->to_json()}, {"methods", {{"method1", method1->to_json()}, {"method2", method2->to_json()}}}}}};
    REQUIRE(e == nj);
}