#include "catch.hpp"
#include "../src/ast/nodes.h"
#include "../src/ast/general/ObjectType.h"
#include "../src/ast/top/TypeclassAst.h"
#include "../src/ast/top/Instance.h"

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
    ast::UExpNode a = ast::Number::make(NumberType::INTEGER, "1", DUMMY_POS, DUMMY_POS);
    ast::UExpNode b = ast::Number::make(NumberType::INTEGER, "2", DUMMY_POS, DUMMY_POS);
    ast::UExpNode c = ast::Number::make(NumberType::INTEGER, "3", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "list"},
                        {"list", {{"elements", nlohmann::json::array({a->to_json(), b->to_json(), c->to_json()})}}}};
    ast::VectorOfExpNodesU v;
    v.push_back(std::move(a));
    v.push_back(std::move(b));
    v.push_back(std::move(c));
    ast::List n(std::move(v), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_call_no_args", "[call]") {
    ast::UExpNode f = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    ast::VectorOfExpNodesU vector;
    nlohmann::json e = {{"type", "call"},
                        {"call", {{"function", f->to_json()}, {"arguments", nlohmann::json::array()}}}};
    ast::Call n(std::move(f), std::move(vector), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_call", "[call]") {
    ast::UExpNode f = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    ast::UExpNode arg_0 = ast::Id::make("bar", DUMMY_POS, DUMMY_POS);
    ast::UExpNode arg_1 = ast::Number::make(NumberType::INTEGER, "3", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "call"},
                        {"call", {{"function", f->to_json()}, {"arguments", {arg_0->to_json(), arg_1->to_json()}}}}};
    ast::VectorOfExpNodesU v;
    v.push_back(std::move(arg_0));
    v.push_back(std::move(arg_1));
    ast::Call n(std::move(f), std::move(v), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_assignment", "[assignment]") {
    ast::UExpNode l = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    ast::UExpNode r = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);

    nlohmann::json e = {{"type",       "assignment"},
                        {"assignment", {{"lvalue", l->to_json()}, {"rvalue", r->to_json()}}}};

    ast::Assignment n(std::move(l), std::move(r), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_declaration_no_type", "[declaration]") {
    ast::UExpNode r = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", {}}, {"expression", r->to_json()}}}};
    ast::Declaration n("foo", nullptr, std::move(r), DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_declaration_with_type", "[declaration]") {
    ast::UExpNode r = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    ast::UTypeNode t = std::make_unique<ast::ObjectType>("Integer");
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", t->to_json()}, {"expression", r->to_json()}}}};
    ast::Declaration n("foo", std::move(t), std::move(r), DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_no_else", "[if]") {
    ast::UExpNode r = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    ast::UTypeNode t = std::make_unique<ast::ObjectType>("Integer");
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", t->to_json()}, {"expression", r->to_json()}}}};
    ast::Declaration n("foo", std::move(t), std::move(r), DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_with_else", "[if]") {
    ast::UExpNode c = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    ast::UExpNode n1 = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Declaration> d = std::make_unique<ast::Declaration>("foo",
                                                                             nullptr,
                                                                             std::move(n1),
                                                                             DUMMY_POS,
                                                                             DUMMY_POS,
                                                                             DUMMY_POS);
    ast::VectorOfNodesU vector;
    vector.push_back(std::move(d));
    std::unique_ptr<ast::Block> t = ast::Block::make(std::move(vector), DUMMY_POS, DUMMY_POS);
    ast::VectorOfNodesU v;
    ast::UExpNode n2 = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v.push_back(std::make_unique<ast::Declaration>("bar", nullptr, std::move(n2), DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<ast::Block> l = ast::Block::make(std::move(v), DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "if"}};
    e["if"]["condition"] = c->to_json();
    e["if"]["then"] = t->to_json();
    e["if"]["elifs"] = nlohmann::json::array();
    e["if"]["else"] = l->to_json();
    ast::If n(std::move(c), std::move(t), {}, std::move(l), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_if_with_elif", "[if]") {
    ast::UExpNode c = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    ast::VectorOfNodesU v;
    ast::UExpNode n1 = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    v.push_back(std::make_unique<ast::Declaration>("foo", nullptr, std::move(n1), DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<ast::Block> t = ast::Block::make(std::move(v), DUMMY_POS, DUMMY_POS);
    ast::VectorOfNodesU v2;
    ast::UExpNode n2 = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v2.push_back(std::make_unique<ast::Declaration>("bar", nullptr, std::move(n2), DUMMY_POS, DUMMY_POS, DUMMY_POS));
    std::unique_ptr<ast::Block> l = ast::Block::make(std::move(v2), DUMMY_POS, DUMMY_POS);
    ast::VectorOfNodesU v3;
    ast::UExpNode n3 = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    v3.push_back(std::make_unique<ast::Declaration>("bar", nullptr, std::move(n3), DUMMY_POS, DUMMY_POS, DUMMY_POS));
    auto elif_body_0 = ast::Block::make(std::move(v3), DUMMY_POS, DUMMY_POS);
    ast::VectorOfNodesU v4;
    ast::UExpNode n4 = ast::Number::make(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    v4.push_back(std::make_unique<ast::Declaration>("foo", nullptr, std::move(n4), DUMMY_POS, DUMMY_POS, DUMMY_POS));
    auto elif_body_1 = ast::Block::make(std::move(v4), DUMMY_POS, DUMMY_POS);

    auto elif_cond_0 = ast::Id::make("a", DUMMY_POS, DUMMY_POS);
    auto elif_cond_1 = ast::Id::make("b", DUMMY_POS, DUMMY_POS);

    nlohmann::json e = {{"type", "if"}};
    e["if"]["condition"] = c->to_json();
    e["if"]["elifs"] = nlohmann::json::array({{{"condition", elif_cond_0->to_json()}, {"then", elif_body_0->to_json()}},
                                              {{"condition", elif_cond_1->to_json()}, {"then", elif_body_1->to_json()}}});
    e["if"]["then"] = t->to_json();
    e["if"]["else"] = l->to_json();

    std::vector<std::pair<ast::UExpNode, ast::UBlock>> elifs;
    elifs.emplace_back(std::move(elif_cond_0), std::move(elif_body_0));
    elifs.emplace_back(std::move(elif_cond_1), std::move(elif_body_1));

    ast::If n(std::move(c), std::move(t), std::move(elifs), std::move(l), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_for", "[for]") {
    ast::VectorOfNodesU v;
    std::unique_ptr<ast::Block> b = ast::Block::make(std::move(v), DUMMY_POS, DUMMY_POS);
    ast::UExpNode exp = ast::Id::make("bar", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "for"},
                        {"for",  {{"var", "foo"}, {"exp", exp->to_json()}, {"body", b->to_json()}}}};
    ast::For n("foo", std::move(exp), std::move(b), DUMMY_POS, DUMMY_POS);
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
    ast::UExpNode ptr;
    ast::Return n(ptr, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "return"},
                        {"return", {{"expression", {}}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_return_with_value", "[return]") {
    ast::UExpNode exp = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",   "return"},
                        {"return", {{"expression", exp->to_json()}}}};
    ast::Return n(exp, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_subscript", "[subscript]") {
    ast::UExpNode i = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    ast::UExpNode s = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",      "subscript"},
                        {"subscript", {{"parent", i->to_json()}, {"child", s->to_json()}}}};
    ast::VectorOfExpNodesU v;
    v.push_back(std::move(s));
    ast::Subscript n(i, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_binop", "[binop]") {
    ast::UExpNode a = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    ast::UExpNode b = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",  "binop"},
                        {"binop", {{"left", a->to_json()}, {"right", b->to_json()}, {"op", op_to_string(OpType::ADD)}}}};
    ast::BinaryOp n(OpType::ADD, std::move(a), std::move(b), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_empty_list", "[empty_list]") {
    auto t = std::make_unique<ast::ObjectType>("Integer");
    nlohmann::json e = {{"type",       "empty_list"},
                        {"empty_list", {{"type", t->to_json()}}}};
    ast::EmptyList n(std::move(t), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_empty_dict", "[empty_dict]") {
    ast::UTypeNode k = std::make_unique<ast::ObjectType>("String");
    ast::UTypeNode v = std::make_unique<ast::ObjectType>("Integer");
    nlohmann::json e = {{"type",       "empty_dict"},
                        {"empty_dict", {{"key_type", k->to_json()}, {"value_type", v->to_json()}}}};
    ast::EmptyDict n(k, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_dict", "[dict]") {
    ast::UExpNode k1 = std::make_unique<ast::String>("foo", DUMMY_POS, DUMMY_POS);
    ast::UExpNode v1 = ast::Number::make(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    ast::UExpNode k2 = std::make_unique<ast::String>("bar", DUMMY_POS, DUMMY_POS);
    ast::UExpNode v2 = ast::Number::make(NumberType::INTEGER, "11", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type", "dict"},
                        {"dict", {{"items", {{{"key", k1->to_json()}, {"value", v1->to_json()}}, {{"key", k2->to_json()}, {"value", v2->to_json()}}}}}}};
    std::vector<std::pair<ast::UExpNode, ast::UExpNode>> d;
    d.emplace_back(std::move(k1), std::move(v1));
    d.emplace_back(std::move(k2), std::move(v2));
    ast::DictNode n(std::move(d), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_member", "[member]") {
    ast::UExpNode p = ast::Id::make("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",   "member"},
                        {"member", {{"parent", p->to_json()}, {"child", "bar"}}}};
    ast::Member n(std::move(p), Token(TokType::ID, "bar", DUMMY_POS));
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_function_no_args", "[function]") {
    ast::VectorOfNodesU v;
    std::unique_ptr<ast::Block> b = ast::Block::make(std::move(v), DUMMY_POS, DUMMY_POS);
    ast::UTypeNode rt = std::make_unique<ast::ObjectType>("Integer");
    nlohmann::json e = {{"type",     "function"},
                        {"function", {{"id", "foo"}, {"parameters", nlohmann::json::array()}, {"body", b->to_json()}, {"return_type", rt->to_json()}}}};
    ast::VectorOfUTypes vt;
    ast::Function n("foo", {}, vt, rt, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_function_args", "[function]") {
    std::unique_ptr<ast::Block> body = ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    ast::UTypeNode a = std::make_unique<ast::ObjectType>("Integer", ast::VectorOfTypes{});
    ast::UTypeNode b = std::make_unique<ast::ObjectType>("String", ast::VectorOfTypes{});
    ast::UTypeNode rt = std::make_unique<ast::ObjectType>("Integer");
    nlohmann::json e = {{"type",     "function"},
                        {"function", {{"id", "foo"}, {"parameters", {{{"id", "bar"}, {"type", a->to_json()}}, {{"id", "baz"}, {"type", b->to_json()}}}}, {"body", body->to_json()}, {"return_type", rt->to_json()}}}};
    ast::VectorOfUTypes vt;
    vt.push_back(std::move(a));
    vt.push_back(std::move(b));
    ast::Function n("foo", {"bar", "baz"}, vt, rt, body, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    REQUIRE(e == nj);
}

TEST_CASE("nodes_while", "[while]") {
    std::unique_ptr<ast::Block> body = ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    ast::UExpNode a = ast::Id::make("bar", DUMMY_POS, DUMMY_POS);
    ast::UExpNode b = ast::Id::make("baz", DUMMY_POS, DUMMY_POS);
    ast::UExpNode cond = std::make_unique<ast::BinaryOp>(OpType::EQ, std::move(a), std::move(b), DUMMY_POS, DUMMY_POS);
    nlohmann::json e = {{"type",  "while"},
                        {"while", {{"condition", cond->to_json()}, {"body", body->to_json()}}}};
    ast::While n(cond, body, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_class_empty", "[class]") {
    std::unordered_map<std::string, ast::UFunctionNode> v;
    ast::ConcreteClassDef n("MyClass", {}, {}, {}, {}, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json members = nlohmann::json::array();
    nlohmann::json methods;
    nlohmann::json static_methods;
    nlohmann::json e = {{"type",  "class"},
                        {"class", {{"id", "MyClass"}, {"members", members}, {"methods", methods}, {"static_methods", static_methods}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_class_full", "[class]") {
    auto t1 = std::make_unique<ast::ObjectType>("Integer");
    auto t2 = std::make_unique<ast::ObjectType>("String");
    VectorOfStrings members_ordered = {"foo", "bar"};
    ast::UTypeNode rt1 = std::make_unique<ast::ObjectType>("Integer");
    ast::UTypeNode rt2 = std::make_unique<ast::ObjectType>("Integer");
    ast::UTypeNode rt3 = std::make_unique<ast::ObjectType>("Integer");
    ast::UTypeNode rt4 = std::make_unique<ast::ObjectType>("Integer");
    std::unique_ptr<ast::Block> b1 = ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Block> b2 = ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Block> b3 = ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Block> b4 = ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    ast::VectorOfUTypes v1;
    ast::VectorOfUTypes v2;
    ast::UFunctionNode method1 = std::make_unique<ast::Function>("method1",
                                                                 VectorOfStrings{},
                                                                 v1,
                                                                 rt1,
                                                                 b1,
                                                                 DUMMY_POS,
                                                                 DUMMY_POS);
    ast::UFunctionNode method2 = std::make_unique<ast::Function>("method2",
                                                                 VectorOfStrings{},
                                                                 v2,
                                                                 rt2,
                                                                 b2,
                                                                 DUMMY_POS,
                                                                 DUMMY_POS);
    nlohmann::json methods = {{"method2", method2->to_json()},
                              {"method1", method1->to_json()}};
    std::unordered_map<std::string, std::unique_ptr<KMethod>> cmethods;
    cmethods["method1"] = std::make_unique<KMethod>(std::move(method1));
    cmethods["method2"] = std::make_unique<KMethod>(std::move(method2));
    nlohmann::json members = {{{"id", "foo"}, {"type", t1->to_json()}},
                              {{"id", "bar"}, {"type", t2->to_json()}}};
    std::vector<std::pair<std::string, ast::UTypeNode>> cmembers;
    cmembers.emplace_back("foo", std::move(t1));
    cmembers.emplace_back("bar", std::move(t2));


    ast::VectorOfUTypes vv1, vv2;
    ast::UFunctionNode smethod1 = std::make_unique<ast::Function>("smethod1",
                                                                  VectorOfStrings{},
                                                                  vv1,
                                                                  rt3,
                                                                  b3,
                                                                  DUMMY_POS,
                                                                  DUMMY_POS);
    ast::UFunctionNode smethod2 = std::make_unique<ast::Function>("smethod2",
                                                                  VectorOfStrings{},
                                                                  vv2,
                                                                  rt4,
                                                                  b4,
                                                                  DUMMY_POS,
                                                                  DUMMY_POS);
    std::unordered_map<std::string, ast::UFunctionNode> cstatic_methods;
    nlohmann::json static_methods = {{"smethod1", smethod1->to_json()},
                                     {"smethod2", smethod2->to_json()}};
    cstatic_methods["smethod1"] = std::move(smethod1);
    cstatic_methods["smethod2"] = std::move(smethod2);
    ast::ConcreteClassDef n("MyClass",
                            {"k", "v"},
                            std::move(cmembers),
                            std::move(cmethods),
                            {},
                            cstatic_methods,
                            DUMMY_POS,
                            DUMMY_POS);
    n.members_ordered = members_ordered;
    nlohmann::json nj = n.to_json();


    nlohmann::json e = {{"type",  "class"},
                        {"class", {{"id", "MyClass"}, {"members", members}, {"methods", methods}, {"static_methods", static_methods}}}};
    REQUIRE(e == nj);
}

TEST_CASE("nodes_typeclass", "[typeclass]") {
    auto method1 = std::make_unique<ast::FunctionType>(ast::VectorOfTypes{new ast::ObjectType("t"),
                                                                          new ast::ObjectType("t")},
                                                       std::make_unique<ast::ObjectType>("Boolean"));
    auto method2 = std::make_unique<ast::FunctionType>(ast::VectorOfTypes{new ast::ObjectType("t"),
                                                                          new ast::ObjectType("t")},
                                                       std::make_unique<ast::ObjectType>("Boolean"));
    nlohmann::json e = {{"type",      "typeclass"},
                        {"typeclass", {{"id", "Comparable"}, {"base_type", "t"}, {"methods", {{"method1", method1->to_json()}, {"method2", method2->to_json()}}}}}};
    std::unordered_map<std::string, ast::UFunctionType> cmethods;
    cmethods["method1"] = std::move(method1);
    cmethods["method2"] = std::move(method2);

    ast::TypeclassAst n("Comparable", "t", std::move(cmethods), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}

TEST_CASE("nodes_instance", "[instance]") {
    // FunctionType* method1 = new FunctionType({new ast::ObjectType("a"), new ast::ObjectType("b")}, new ast::ObjectType("Boolean"));
    // FunctionType* method2 = new FunctionType({new ast::ObjectType("c"), new ast::ObjectType("c")}, new ast::ObjectType("Boolean"));
    ast::UTypeNode rt1 = std::make_unique<ast::ObjectType>("Integer");
    ast::UTypeNode rt2 = std::make_unique<ast::ObjectType>("Integer");
    std::unique_ptr<ast::Block> b1 = ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Block> b2 = ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    ast::VectorOfUTypes v1;
    ast::VectorOfUTypes v2;
    auto method1 = std::make_unique<ast::Function>("method1", VectorOfStrings{}, v1, rt1, b1, DUMMY_POS, DUMMY_POS);
    auto method2 = std::make_unique<ast::Function>("method2", VectorOfStrings{}, v2, rt2, b2, DUMMY_POS, DUMMY_POS);

    auto bt = std::make_unique<ast::ObjectType>("SomeType");
    nlohmann::json e = {{"type",     "instance"},
                        {"instance", {{"id", "Comparable"}, {"base_type", bt->to_json()}, {"methods", {{"method1", method1->to_json()}, {"method2", method2->to_json()}}}}}};
    std::unordered_map<std::string, ast::UFunctionNode> cmethods;
    cmethods["method1"] = std::move(method1);
    cmethods["method2"] = std::move(method2);
    ast::Instance n("Comparable", std::move(bt), std::move(cmethods), DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();

    REQUIRE(e == nj);
}