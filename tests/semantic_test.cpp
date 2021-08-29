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

static const ObjectType NO_TYPE(".None");

static const TextPosition& _POS = {1, 1};

Compiler c_analyze(std::string code) {
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
    Compiler c(project_dir, project_output_dir, output_name, lib_path, is_lib, version);
    c.pre();
    return c;
}

TEST_CASE("semantic_output_basic_function", "[checker]") {
    std::string code = "fun foo()->Integer{return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
}

TEST_CASE("semantic_output_basic_declaration", "[checker]") {
    std::string code = "fun main()->Integer{var x = 9;return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_declaration((ast::Declaration&) *((std::unique_ptr<ast::Function>&) module.ast->nodes[0])->body->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
}

TEST_CASE("semantic_output_list", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,1];return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}


TEST_CASE("semantic_output_empty_dict", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {}::[Integer,String];return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_dict", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",9:\"nine\"};return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.visit_function((ast::Function&) *module.ast->nodes[0]);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_int_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9;return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = *((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type == ObjectType("Integer"));
}

TEST_CASE("semantic_output_bool_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = false;return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = *((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type == ObjectType("Boolean"));
}

TEST_CASE("semantic_output_list_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,1];return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = *((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type == ObjectType("List", {new ObjectType("Integer")}));
}

TEST_CASE("semantic_output_empty_list_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = []::String;return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = *((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type == ObjectType("List", {new ObjectType("String")}));
}

TEST_CASE("semantic_output_empty_dict_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {}::[Integer,String];return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = *((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type ==
            ObjectType("Dict", {new ObjectType("Integer"), new ObjectType("String")}));
}


TEST_CASE("semantic_output_dict_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {\"one\":1,\"two\":2};return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = *((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type ==
            ObjectType("Dict", {new ObjectType("String"), new ObjectType("Integer")}));
}

TEST_CASE("semantic_output_float_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9.5;return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    ast::Node& expression = *((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((EntityValue&) (info->entity.get())).value->metatype == Meta::CLASS);
    REQUIRE(*((EntityValue&) (info->entity.get())).value->type == ObjectType("Float"));
}

// TEST_CASE("semantic_output_none_literal", "[checker]") {
//     std::string code = "fun foo()->Integer{var x = none;return 0;}";
//
//     Compiler c = c_analyze(code);
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




TEST_CASE("semantic_output_member", "[checker]") {
    std::string code = "class Foo{foo: String;}\nfun bar(f: Foo)->Integer{var x : String = f.foo;return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    CHECK(!checker.error_reporter.failed);
    CHECK(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_binop", "[checker]") {
    std::string code = "fun bar()->Integer{var x = 2 + 5;return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();

    ast::Node& expression = *((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
    CHECK(info->entity.get().type == E_TYPE::VALUE);
    EntityValue& entity_value = (EntityValue&) (info->entity.get());
    CHECK(entity_value.value->metatype == Meta::CLASS);
    CHECK(*entity_value.value->type == ObjectType("Integer"));
}

TEST_CASE("semantic_output_boolop", "[checker]") {
    std::string code = "fun bar()->Integer{var x = 2 < 5;return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();

    ast::Node& expression = *((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
    CHECK(info->entity.get().type == E_TYPE::VALUE);
    EntityValue& entity_value = (EntityValue&) (info->entity.get());
    CHECK(entity_value.value->metatype == Meta::CLASS);
    CHECK(*entity_value.value->type == ObjectType("Boolean"));
}

TEST_CASE("semantic_output_subscript", "[checker]") {
    std::string code = "fun bar()->Integer{var x = [1,3,4][2] ;return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();

    ast::Node& expression = *((ast::Declaration&) *((ast::Function&) *module.ast->nodes[0]).body->nodes[0]).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_call_no_args", "[checker]") {
    std::string code = "fun bar()->Integer{return 0;}\nfun foo()->Integer{var x : Integer = bar();return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_call_args", "[checker]") {
    std::string code = "fun bar(a: Integer, b: String)->Integer{return 0;}\nfun foo()->Integer{var x : Integer = bar(8, \"Hello\");return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_union_ok_1", "[checker]") {
    std::string code = "fun foo()->Integer{var x : Union[Integer, String] = 3;return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_union_ok_2", "[checker]") {
    std::string code = "fun foo()->Integer{var x : Union[Integer, String] = \"String\";return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_while", "[checker]") {
    std::string code = "fun foo()->Integer{while true {var x = 1;}return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_if", "[checker]") {
    std::string code = "fun foo()->Integer{if true {var x = 1;}return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_enum", "[checker]") {
    std::string code = "enum Foo {a, c}\n fun foo()->Integer{var x = Foo.a; var y = Foo.c; return 0;}";

    Compiler c = c_analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(*c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}