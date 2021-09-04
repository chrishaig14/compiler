#include "catch.hpp"
#include "../src/scanner/Scanner.h"
#include "../src/parser/Parser.h"
#include "../src/ast/UnaryOp.h"
#include "../src/semantic/GlobalProcessor.h"
#include "../src/semantic/Checker.h"
#include "../src/compiler/Compiler.h"
#include "../src/compiler/analyze.h"
#include "../src/semantic/errors/ErrorTypeMismatch.h"
#include "../src/semantic/errors/ErrorRedeclared.h"
#include "../src/semantic/errors/ErrorExpectedExpression.h"
#include "../src/semantic/errors/ErrorNoMember.h"
#include "../src/semantic/errors/ErrorClassNoMember.h"
#include "../src/semantic/errors/ErrorNoMemberSuggestions.h"
#include "../src/semantic/errors/ErrorClassNoMethodForOp.h"
#include "../src/semantic/errors/ErrorObjectNoSpecialMethod.h"
#include "../src/semantic/errors/ErrorEnumNoValue.h"

const ObjectType NO_TYPE(".None");

const TextPosition& _POS = {1, 1};

std::unique_ptr<Compiler> analyze(std::string code) {
    const std::string& tmp_in = "tmp_in";
    mkdir(tmp_in.c_str(), 0700);
    const std::string& tmp_out = "tmp_out";
    mkdir(tmp_out.c_str(), 0700);
    std::ofstream tmp_req(tmp_in + "/requirements.txt");
    tmp_req << "core==1.0.0";
    tmp_req.flush();
    std::ofstream tmp(tmp_in + "/tmp.xl");
    tmp << code;
    tmp.flush();
    std::string project_dir = tmp_in;
    std::string project_output_dir = tmp_out;
    std::string output_name = "test";
    std::string lib_path = "/home/chris/CLionProjects/compiler/lib";
    bool is_lib = false;
    std::string version = "1.0.0";
    auto c = std::make_unique<Compiler>(project_dir, project_output_dir, output_name, lib_path, is_lib, version);
    c->pre();
    return c;
}

TEST_CASE("basic_function", "[checker]") {
    std::string code = "fun foo()->Integer{return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
}

TEST_CASE("basic_function_bad_return_type", "[checker]") {
    std::string code = "fun foo()->Integer{return false;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
    Error& error = *checker.error_reporter.errors.back();
    ast::Boolean node(false, _POS, _POS);
    ObjectType expected("Integer");
    ErrorTypeMismatch exp(expected, node, *checker.entity_from_type(ObjectType("Boolean")));
    REQUIRE(error == exp);

}

TEST_CASE("basic_declaration", "[checker]") {
    std::string code = "fun main()->Integer{var x = 9;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_declaration((ast::Declaration&) *((std::unique_ptr<ast::Function>&) module.ast->nodes[0])->body->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
}

TEST_CASE("basic_declaration_bad_type", "[checker]") {
    std::string code = "fun foo()->Integer{var x: Boolean = 9;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_declaration((ast::Declaration&) *((std::unique_ptr<ast::Function>&) module.ast->nodes[0])->body->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
    Error& error = *checker.error_reporter.errors.back();
    ast::Number node(NumberType::INTEGER, "9", _POS, _POS);
    ObjectType expected("Boolean");
    ErrorTypeMismatch exp(expected, node, *checker.entity_from_type(ObjectType("Integer")));
    REQUIRE(error == exp);
}

TEST_CASE("error_redeclared", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9; var x = 10;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
    Error& error = *checker.error_reporter.errors.back();
    ast::Number node(NumberType::INTEGER, "9", _POS, _POS);
    ObjectType expected("Boolean");
    ErrorRedeclared exp("x",
                        (ast::Declaration&) *((std::unique_ptr<ast::Function>&) module.ast->nodes[0])->body->nodes[1]);
    REQUIRE(error == exp);
}

TEST_CASE("list_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,1];return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("list_bad", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,\"a\"];return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
    Error& error = *checker.error_reporter.errors.back();


    ast::String node("a", _POS, _POS);
    ObjectType expected("Integer");
    ErrorTypeMismatch exp(expected, node, *checker.entity_from_type(ObjectType("String")));

    REQUIRE(error == exp);
}

TEST_CASE("empty_dict_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {}::[Integer,String];return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("dict_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",9:\"nine\"};return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("dict_key_type_error", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",\"nine\":\"ten\"};return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
}

TEST_CASE("dict_value_type_error", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",9:false};return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
}


TEST_CASE("int_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type == ObjectType("Integer"));
}

TEST_CASE("bool_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = false;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type == ObjectType("Boolean"));
}

TEST_CASE("list_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,1];return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type == ObjectType("List", {new ObjectType("Integer")}));
}

TEST_CASE("empty_list_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = []::String;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type == ObjectType("List", {new ObjectType("String")}));
}

TEST_CASE("empty_dict_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {}::[Integer,String];return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type ==
            ObjectType("Dict", {new ObjectType("Integer"), new ObjectType("String")}));
}


TEST_CASE("dict_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {\"one\":1,\"two\":2};return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type ==
            ObjectType("Dict", {new ObjectType("String"), new ObjectType("Integer")}));
}

TEST_CASE("float_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9.5;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type == ObjectType("Float"));
}

// TEST_CASE("none_literal", "[checker]") {
//     std::string code = "fun foo()->Integer{var x = none;return 0;}";
//
//     std::unique_ptr<Compiler> cp =  analyze(code);Compiler& c = *cp;
//     Module& module = *c.root_package->units["tmp"].module;
//     analyze_module_result(module, *c.top_package);
//     Checker checker(*c.top_package,module);
//     ast::Node* expression = ((ast::DeclarationNode*) ((FunctionNode&)* module.ast->nodes[0]).body->nodes[0])->expression;
//     USemanticInfo info = checker.dispatch_rvalue(expression);
//
//     REQUIRE(!checker.error_reporter.failed);
//     REQUIRE(checker.error_reporter.errors.size() == 0);
//     REQUIRE(info->entity.get().type == E_TYPE::VALUE);
//     REQUIRE(((EntityValue*)(info->entity))->value->metatype == Meta::CLASS);
//     REQUIRE(*((EntityValue*)(info->entity))->value->type == ObjectType("NoneType"));
// }

TEST_CASE("decl_error_expected_expression", "[checker]") {
    std::string code = "fun foo()->Integer{var x = Integer;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    ast::Function& function_node = (ast::Function&) *module.ast->nodes[0];
    ast::Declaration& declaration_node = (ast::Declaration&) *function_node.body->nodes[0];
    checker.visit_function(function_node);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);

    Error& error = *checker.error_reporter.errors.back();
    Class cl;
    EntityClass ec(&cl);
    ErrorExpectedExpression exp(ec, declaration_node.expression);
    REQUIRE(error == exp);
}

TEST_CASE("error_no_member", "[checker]") {
    std::string code = "class Foo{foo: Integer;}\nfun bar(f: Foo)->Integer{var x = f.lala;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    ast::Function& function_node = (ast::Function&) *module.ast->nodes[1];
    ast::Declaration& declaration_node = (ast::Declaration&) *function_node.body->nodes[0];
    checker.visit_root(*module.ast);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);

    Error& error = *checker.error_reporter.errors.back();
    Flirpin clazz_flirpin = module.get(Path("Foo"));
    REQUIRE(clazz_flirpin.type == F_TYPE::CLASS);
    ObjectType type = ObjectType("Foo");
    ErrorNoMemberSuggestions exp(type, (ast::Member&) declaration_node.expression, *clazz_flirpin.clazz);
    REQUIRE(error == exp);
}

TEST_CASE("member_ok", "[checker]") {
    std::string code = "class Foo{foo: String;}\nfun bar(f: Foo)->Integer{var x : String = f.foo;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    CHECK(!checker.error_reporter.failed);
    CHECK(checker.error_reporter.errors.empty());
}

TEST_CASE("binop_ok", "[checker]") {
    std::string code = "fun bar()->Integer{var x = 2 + 5;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();

    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
    CHECK(info->entity.get().type == E_TYPE::VALUE);
    EntityValue& entity_value = (EntityValue&) (info->entity.get());
    CHECK(entity_value.value->metatype == Meta::CLASS);
    CHECK(*entity_value.value->type == ObjectType("Integer"));
}

TEST_CASE("boolop_ok", "[checker]") {
    std::string code = "fun bar()->Integer{var x = 2 < 5;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();

    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
    CHECK(info->entity.get().type == E_TYPE::VALUE);
    EntityValue& entity_value = (EntityValue&) (info->entity.get());
    CHECK(entity_value.value->metatype == Meta::CLASS);
    CHECK(*entity_value.value->type == ObjectType("Boolean"));
}


TEST_CASE("binop_type_error", "[checker]") {
    std::string code = "fun bar()->Integer{var x = 2 + \"Hello\";return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();

    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);

    Error& error = *checker.error_reporter.errors.back();
    ast::String node("Hello", _POS, _POS);
    ObjectType expected("Integer");
    ErrorTypeMismatch exp(expected, node, *checker.entity_from_type(ObjectType("String")));
    REQUIRE(error == exp);
}


TEST_CASE("binop_error", "[checker]") {
    std::string code = "fun bar()->Integer{var x = \"Hello\" - \"Bye\" ;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();

    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);

    Error& error = *checker.error_reporter.errors.back();
    UNode left = std::make_unique<ast::String>("Hello", _POS, _POS);
    UNode right = std::make_unique<ast::String>("Bye", _POS, _POS);
    ast::BinaryOp node(OpType::SUB, std::move(left), std::move(right), _POS, _POS);
    ObjectType expected("Integer");
    ErrorClassNoMethodForOp exp("String", "__sub__", node);
    REQUIRE(error == exp);
}

TEST_CASE("subscript_ok", "[checker]") {
    std::string code = "fun bar()->Integer{var x = [1,3,4][2] ;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();

    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("subscript_index_type_error", "[checker]") {
    std::string code = "fun bar()->Integer{var x = [1,3,4][\"foo\"] ;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();

    ast::Node& expression = ((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);

    Error& error = *checker.error_reporter.errors.back();
    ast::String node("foo", _POS, _POS);
    ObjectType expected("Integer");
    ErrorTypeMismatch exp(expected, node, *checker.entity_from_type(ObjectType("String")));
    REQUIRE(error == exp);
}

TEST_CASE("subscript_no_method_error", "[checker]") {
    std::string code = "class Foo{foo: String;}\nfun bar(f: Foo)->Integer{var x = f[1];return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);

    Error& error = *checker.error_reporter.errors.back();
    ast::String left("Hello", _POS, _POS);
    ast::String right("Bye", _POS, _POS);
    UNode p_node = ast::Id::make("f", _POS, _POS);
    VectorOfNodesU v;
    v.push_back(ast::Number::make(NumberType::INTEGER, "1", _POS, _POS));
    ast::Subscript node(p_node, v, _POS, _POS);
    ObjectType expected("Integer");
    ObjectType type("Foo");
    ErrorObjectNoSpecialMethod exp(type, "__get_item__", node);
    REQUIRE(error == exp);
}

TEST_CASE("call_no_args_ok", "[checker]") {
    std::string code = "fun bar()->Integer{return 0;}\nfun foo()->Integer{var x : Integer = bar();return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("call_args_ok", "[checker]") {
    std::string code = "fun bar(a: Integer, b: String)->Integer{return 0;}\nfun foo()->Integer{var x : Integer = bar(8, \"Hello\");return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("call_args_type_error", "[checker]") {
    std::string code = "fun bar(a: Integer, b: String)->Integer{return 0;}\nfun foo()->Integer{var x : Integer = bar(\"Hello\",\"Bye\");return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);


    Error& error = *checker.error_reporter.errors.back();
    ast::String node("Hello", _POS, _POS);
    ObjectType expected("Integer");
    ErrorTypeMismatch exp(expected, node, *checker.entity_from_type(ObjectType("String")));
    REQUIRE(error == exp);
}

TEST_CASE("union_ok_1", "[checker]") {
    std::string code = "fun foo()->Integer{var x : Union[Integer, String] = 3;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("union_ok_2", "[checker]") {
    std::string code = "fun foo()->Integer{var x : Union[Integer, String] = \"String\";return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("union_error", "[checker]") {
    std::string code = "fun foo()->Integer{var x : Union[Integer, String] = false;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);

    Error& error = *checker.error_reporter.errors.back();
    ast::Boolean node(false, _POS, _POS);
    ObjectType expected("Union", {new ObjectType("Integer"), new ObjectType("String")});
    ErrorTypeMismatch exp(expected, node, *checker.entity_from_type(ObjectType("Boolean")));
    REQUIRE(error == exp);
}

TEST_CASE("while_ok", "[checker]") {
    std::string code = "fun foo()->Integer{while true {var x = 1;}return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("while_boolean_error", "[checker]") {
    std::string code = "fun foo()->Integer{while 5 {var x = 1;}return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);


    Error& error = *checker.error_reporter.errors.back();
    ast::Number node(NumberType::INTEGER, "5", _POS, _POS);
    ObjectType expected("Boolean");
    ErrorTypeMismatch exp(expected, node, *checker.entity_from_type(ObjectType("Integer")));
    REQUIRE(error == exp);
}

TEST_CASE("if_ok", "[checker]") {
    std::string code = "fun foo()->Integer{if true {var x = 1;}return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("if_boolean_error", "[checker]") {
    std::string code = "fun foo()->Integer{if 5 {var x = 1;}return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);


    Error& error = *checker.error_reporter.errors.back();
    ast::Number node(NumberType::INTEGER, "5", _POS, _POS);
    ObjectType expected("Boolean");
    ErrorTypeMismatch exp(expected, node, *checker.entity_from_type(ObjectType("Integer")));
    REQUIRE(error == exp);
}

TEST_CASE("enum_error", "[checker]") {
    std::string code = "enum Foo {a, c}\n fun foo()->Integer{var x = Foo.b;return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);


    Error& error = *checker.error_reporter.errors.back();
    UNode u = ast::Id::make("Foo", _POS, _POS);
    ast::Member node(std::move(u), Token(TokType::ID, "b", _POS));
    ObjectType expected("Boolean");
    ErrorEnumNoValue exp("Foo", "b", node, nullptr);
    REQUIRE(error == exp);
}


TEST_CASE("enum_ok", "[checker]") {
    std::string code = "enum Foo {a, c}\n fun foo()->Integer{var x = Foo.a; var y = Foo.c; return 0;}";

    std::unique_ptr<Compiler> cp = analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}