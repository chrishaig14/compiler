#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../src/nodes/nodes.h"
#include "../src/nodes/ObjectType.h"

const TextPosition DUMMY_POS = {0, 0};

TEST_CASE("number_integer", "[number]") {
    NumberNode n(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "number"},
                        {"number", {{"num_type", "integer"}, {"str", "7"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("number_float", "[number]") {
    NumberNode n(NumberType::FLOAT, "7.98", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "number"},
                        {"number", {{"num_type", "float"}, {"str", "7.98"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("number_double", "[number]") {
    NumberNode n(NumberType::DOUBLE, "7.98", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "number"},
                        {"number", {{"num_type", "double"}, {"str", "7.98"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("string", "[string]") {
    StringNode n("hello", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "string"},
                        {"string", {{"str", "hello"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("boolean", "[boolean]") {
    BooleanNode n(true, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",    "boolean"},
                        {"boolean", {{"value", true}}}};
    REQUIRE(e == nj);
}

TEST_CASE("id", "[id]") {
    IdNode n("foo", DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "id"},
                        {"id",   {{"_id", "foo"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("list", "[list]") {
    NumberNode* a = new NumberNode(NumberType::INTEGER, "1", DUMMY_POS, DUMMY_POS);
    NumberNode* b = new NumberNode(NumberType::INTEGER, "2", DUMMY_POS, DUMMY_POS);
    NumberNode* c = new NumberNode(NumberType::INTEGER, "3", DUMMY_POS, DUMMY_POS);
    ListNode n({a, b, c}, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "list"},
                        {"list", {{"elements", nlohmann::json::array({a->to_json(), b->to_json(), c->to_json()})}}}};
    REQUIRE(e == nj);
}

TEST_CASE("call_no_args", "[call]") {
    IdNode* f = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    CallNode n(f, {}, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "call"},
                        {"call", {{"function", f->to_json()}, {"arguments", nlohmann::json::array()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("call", "[call]") {
    IdNode* f = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    IdNode* arg_0 = new IdNode("bar", DUMMY_POS, DUMMY_POS);
    NumberNode* arg_1 = new NumberNode(NumberType::INTEGER, "3", DUMMY_POS, DUMMY_POS);
    CallNode n(f, {arg_0, arg_1}, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "call"},
                        {"call", {{"function", f->to_json()}, {"arguments", {arg_0->to_json(), arg_1->to_json()}}}}};
    REQUIRE(e == nj);
}

TEST_CASE("assignment", "[assignment]") {
    IdNode* l = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    NumberNode* r = new NumberNode(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    AssignmentNode n(l, r, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",       "assignment"},
                        {"assignment", {{"lvalue", l->to_json()}, {"rvalue", r->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("declaration_no_type", "[declaration]") {
    NumberNode* r = new NumberNode(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    DeclarationNode n("foo", nullptr, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", {}}, {"expression", r->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("declaration_with_type", "[declaration]") {
    NumberNode* r = new NumberNode(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    ObjectType* t = new ObjectType("Integer");
    DeclarationNode n("foo", t, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", t->to_json()}, {"expression", r->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("if_no_else", "[if]") {
    NumberNode* r = new NumberNode(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS);
    ObjectType* t = new ObjectType("Integer");
    DeclarationNode n("foo", t, r, DUMMY_POS, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",        "declaration"},
                        {"declaration", {{"identifier", "foo"}, {"type", t->to_json()}, {"expression", r->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("if_with_else", "[if]") {
    IdNode* c = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    BlockNode* t = new BlockNode({new DeclarationNode("foo",
                                                      nullptr,
                                                      new NumberNode(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS),
                                                      DUMMY_POS,
                                                      DUMMY_POS,
                                                      DUMMY_POS)}, DUMMY_POS, DUMMY_POS);
    BlockNode* l = new BlockNode({new DeclarationNode("bar",
                                                      nullptr,
                                                      new NumberNode(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS),
                                                      DUMMY_POS,
                                                      DUMMY_POS,
                                                      DUMMY_POS)}, DUMMY_POS, DUMMY_POS);
    IfNode n(c, t, {}, l, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "if"}};
    e["if"]["condition"] = c->to_json();
    e["if"]["then"] = t->to_json();
    e["if"]["elifs"] = nlohmann::json::array();
    e["if"]["else"] = l->to_json();
    REQUIRE(e == nj);
}

TEST_CASE("if_with_elif", "[if]") {
    IdNode* c = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    BlockNode* t = new BlockNode({new DeclarationNode("foo",
                                                      nullptr,
                                                      new NumberNode(NumberType::INTEGER, "7", DUMMY_POS, DUMMY_POS),
                                                      DUMMY_POS,
                                                      DUMMY_POS,
                                                      DUMMY_POS)}, DUMMY_POS, DUMMY_POS);
    BlockNode* l = new BlockNode({new DeclarationNode("bar",
                                                      nullptr,
                                                      new NumberNode(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS),
                                                      DUMMY_POS,
                                                      DUMMY_POS,
                                                      DUMMY_POS)}, DUMMY_POS, DUMMY_POS);
    BlockNode* elif_body_0 = new BlockNode({new DeclarationNode("bar",
                                                                nullptr,
                                                                new NumberNode(NumberType::INTEGER,
                                                                               "9",
                                                                               DUMMY_POS,
                                                                               DUMMY_POS),
                                                                DUMMY_POS,
                                                                DUMMY_POS,
                                                                DUMMY_POS)}, DUMMY_POS, DUMMY_POS);
    BlockNode* elif_body_1 = new BlockNode({new DeclarationNode("foo",
                                                                nullptr,
                                                                new NumberNode(NumberType::INTEGER,
                                                                               "7",
                                                                               DUMMY_POS,
                                                                               DUMMY_POS),
                                                                DUMMY_POS,
                                                                DUMMY_POS,
                                                                DUMMY_POS)}, DUMMY_POS, DUMMY_POS);

    IdNode* elif_cond_0 = new IdNode("a", DUMMY_POS, DUMMY_POS);
    IdNode* elif_cond_1 = new IdNode("b", DUMMY_POS, DUMMY_POS);
    IfNode n(c,
             t,
             {{elif_cond_0, elif_body_0},
              {elif_cond_1, elif_body_1}},
             l,
             DUMMY_POS,
             DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "if"}};
    e["if"]["condition"] = c->to_json();
    e["if"]["elifs"] = nlohmann::json::array({{{"condition", elif_cond_0->to_json()}, {"then", elif_body_0->to_json()}},
                                              {{"condition", elif_cond_1->to_json()}, {"then", elif_body_1->to_json()}}});
    e["if"]["then"] = t->to_json();
    e["if"]["else"] = l->to_json();
    REQUIRE(e == nj);
}

TEST_CASE("for", "[for]") {
    BlockNode* b = new BlockNode({}, DUMMY_POS, DUMMY_POS);
    IdNode* exp = new IdNode("bar", DUMMY_POS, DUMMY_POS);
    ForNode n("foo", exp, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "for"},
                        {"for",  {{"var", "foo"}, {"exp", exp->to_json()}, {"body", b->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("none", "[none]") {
    NoneNode n(DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "none"}};
    REQUIRE(e == nj);
}

TEST_CASE("break", "[break]") {
    BreakNode n(DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "break"}};
    REQUIRE(e == nj);
}

TEST_CASE("continue", "[continue]") {
    ContinueNode n(DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "continue"}};
    REQUIRE(e == nj);
}

TEST_CASE("return_no_value", "[return]") {
    ReturnNode n(nullptr, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "return"},
                        {"return", {{"expression", {}}}}};
    REQUIRE(e == nj);
}

TEST_CASE("return_with_value", "[return]") {
    IdNode* exp = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    ReturnNode n(exp, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "return"},
                        {"return", {{"expression", exp->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("subscript", "[subscript]") {
    IdNode* i = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    NumberNode* s = new NumberNode(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    SubscriptNode n(i, {s}, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",      "subscript"},
                        {"subscript", {{"parent", i->to_json()}, {"child", s->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("binop", "[binop]") {
    IdNode* a = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    NumberNode* b = new NumberNode(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    BinopNode n(OpType::ADD, a, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",  "binop"},
                        {"binop", {{"left", a->to_json()}, {"right", b->to_json()}, {"op", op_to_string(OpType::ADD)}}}};
    REQUIRE(e == nj);
}

TEST_CASE("empty_list", "[empty_list]") {
    ObjectType* t = new ObjectType("Integer", {});
    EmptyListNode n(t, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",       "empty_list"},
                        {"empty_list", {{"type", t->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("empty_dict", "[empty_dict]") {
    ObjectType* k = new ObjectType("String", {});
    ObjectType* v = new ObjectType("Integer", {});
    EmptyDictNode n(k, v, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",       "empty_dict"},
                        {"empty_dict", {{"key_type", k->to_json()}, {"value_type", v->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("dict", "[dict]") {
    StringNode* k1 = new StringNode("foo", DUMMY_POS, DUMMY_POS);
    NumberNode* v1 = new NumberNode(NumberType::INTEGER, "9", DUMMY_POS, DUMMY_POS);
    StringNode* k2 = new StringNode("bar", DUMMY_POS, DUMMY_POS);
    NumberNode* v2 = new NumberNode(NumberType::INTEGER, "11", DUMMY_POS, DUMMY_POS);
    DictNode n({{k1, v1},
                {k2, v2}}, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type", "dict"},
                        {"dict", {{"items", {{{"key", k1->to_json()}, {"value", v1->to_json()}}, {{"key", k2->to_json()}, {"value", v2->to_json()}}}}}}};
    REQUIRE(e == nj);
}

TEST_CASE("member", "[member]") {
    IdNode* p = new IdNode("foo", DUMMY_POS, DUMMY_POS);
    MemberNode n(p, Token(TokType::ID, "bar", DUMMY_POS));
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",   "member"},
                        {"member", {{"parent", p->to_json()}, {"child", "bar"}}}};
    REQUIRE(e == nj);
}

TEST_CASE("function_no_args", "[function]") {
    BlockNode* b = new BlockNode({}, DUMMY_POS, DUMMY_POS);
    ObjectType* rt = new ObjectType("Integer", {});
    FunctionNode n("foo", {}, {}, rt, b, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",     "function"},
                        {"function", {{"id", "foo"}, {"parameters", nlohmann::json::array()}, {"body", b->to_json()}, {"return_type", rt->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("function_args", "[function]") {
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

TEST_CASE("while", "[while]") {
    BlockNode* body = new BlockNode({}, DUMMY_POS, DUMMY_POS);
    IdNode* a = new IdNode("bar", DUMMY_POS, DUMMY_POS);
    IdNode* b = new IdNode("baz", DUMMY_POS, DUMMY_POS);
    BoolOpNode* cond = new BoolOpNode(BoolOp::EQ, a, b, DUMMY_POS, DUMMY_POS);
    WhileNode n(cond, body, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json e = {{"type",  "while"},
                        {"while", {{"condition", cond->to_json()}, {"body", body->to_json()}}}};
    REQUIRE(e == nj);
}

TEST_CASE("class_empty", "[class]") {
    ClassNode n("MyClass", {}, {}, {}, {}, {}, DUMMY_POS, DUMMY_POS);
    nlohmann::json nj = n.to_json();
    nlohmann::json members = nlohmann::json::array();
    nlohmann::json methods;
    nlohmann::json static_methods;
    nlohmann::json e = {{"type",  "class"},
                        {"class", {{"id", "MyClass"}, {"members", members}, {"methods", methods}, {"static_methods", static_methods}}}};
    REQUIRE(e == nj);
}

TEST_CASE("class_full", "[class]") {
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