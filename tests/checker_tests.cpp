#include "catch.hpp"
#include "../src/scanner/Scanner.h"
#include "../src/parser/Parser.h"
#include "../src/nodes/UnaryOpNode.h"
#include "../src/semantic/GlobalProcessor.h"
#include "../src/semantic/Checker.h"
#include "../src/compiler/Compiler.h"
#include "../src/compiler/analyze.h"
#include "../src/semantic/errors/ErrorTypeMismatch.h"
#include "../src/semantic/errors/ErrorRedeclared.h"

const ObjectType NO_TYPE(".None");

Compiler analyze(std::string code) {
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

TEST_CASE("basic_function", "[checker]") {
    std::string code = "fun foo()->Integer{return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_function(*(FunctionNode*) module.ast->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
}

TEST_CASE("basic_function_bad_return_type", "[checker]") {
    std::string code = "fun foo()->Integer{return false;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_function(*(FunctionNode*) module.ast->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
    Error& error = *checker.error_reporter.errors.back();
    BooleanNode node(false, {1, 1}, {1, 1});
    ObjectType expected("Integer");
    ErrorTypeMismatch exp(expected, node, entity_from_type(ObjectType("Boolean")));
    REQUIRE(error == exp);

}

TEST_CASE("basic_declaration", "[checker]") {
    std::string code = "fun main()->Integer{var x = 9;return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_declaration(*(DeclarationNode*) ((FunctionNode*) module.ast->nodes[0])->body->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
}

TEST_CASE("basic_declaration_bad_type", "[checker]") {
    std::string code = "fun foo()->Integer{var x: Boolean = 9;return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_declaration(*(DeclarationNode*) ((FunctionNode*) module.ast->nodes[0])->body->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
    Error& error = *checker.error_reporter.errors.back();
    NumberNode node(NumberType::INTEGER, "9", {1, 1}, {1, 1});
    ObjectType expected("Boolean");
    ErrorTypeMismatch exp(expected, node, entity_from_type(ObjectType("Integer")));
    REQUIRE(error == exp);
}

TEST_CASE("bad_binop_type", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9 + \"a\";return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_declaration(*(DeclarationNode*) ((FunctionNode*) module.ast->nodes[0])->body->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
    Error& error = *checker.error_reporter.errors.back();
    NumberNode node(NumberType::INTEGER, "9", {1, 1}, {1, 1});
    ObjectType expected("Boolean");
    ErrorTypeMismatch exp(expected, node, entity_from_type(ObjectType("Integer")));
    REQUIRE(error == exp);
}

TEST_CASE("error_redeclared", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9; var x = 10;return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_function(*(FunctionNode*) module.ast->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
    Error& error = *checker.error_reporter.errors.back();
    NumberNode node(NumberType::INTEGER, "9", {1, 1}, {1, 1});
    ObjectType expected("Boolean");
    ErrorRedeclared exp("x", *(DeclarationNode*) ((FunctionNode*) module.ast->nodes[0])->body->nodes[1]);
    REQUIRE(error == exp);
}

TEST_CASE("list_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,1];return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_function(*(FunctionNode*) module.ast->nodes[0]);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("list_bad", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,\"a\"];return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_function(*(FunctionNode*) module.ast->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
    Error& error = *checker.error_reporter.errors.back();


    StringNode node("a", {1, 1}, {1, 1});
    ObjectType expected("Integer");
    ErrorTypeMismatch exp(expected, node, entity_from_type(ObjectType("String")));

    REQUIRE(error == exp);
}

TEST_CASE("empty_dict_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {}::[Integer,String];return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_function(*(FunctionNode*) module.ast->nodes[0]);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("dict_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",9:\"nine\"};return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_function(*(FunctionNode*) module.ast->nodes[0]);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("dict_key_type_error", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",\"nine\":\"ten\"};return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_function(*(FunctionNode*) module.ast->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
}

TEST_CASE("dict_value_type_error", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",9:false};return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    checker.visit_function(*(FunctionNode*) module.ast->nodes[0]);

    REQUIRE(checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 1);
}


TEST_CASE("int_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9;return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    Node* expression = ((DeclarationNode*) (*(FunctionNode*) module.ast->nodes[0]).body->nodes[0])->expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.type == E_TYPE::VALUE);
    REQUIRE(info->entity.value->metatype == Meta::CLASS);
    REQUIRE(*info->entity.value->type == ObjectType("Integer"));
}

TEST_CASE("bool_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = false;return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    Node* expression = ((DeclarationNode*) (*(FunctionNode*) module.ast->nodes[0]).body->nodes[0])->expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.type == E_TYPE::VALUE);
    REQUIRE(info->entity.value->metatype == Meta::CLASS);
    REQUIRE(*info->entity.value->type == ObjectType("Boolean"));
}

TEST_CASE("list_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,1];return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    Node* expression = ((DeclarationNode*) (*(FunctionNode*) module.ast->nodes[0]).body->nodes[0])->expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.type == E_TYPE::VALUE);
    REQUIRE(info->entity.value->metatype == Meta::CLASS);
    REQUIRE(*info->entity.value->type == ObjectType("List", {new ObjectType("Integer")}));
}

TEST_CASE("empty_list_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = []::String;return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    Node* expression = ((DeclarationNode*) (*(FunctionNode*) module.ast->nodes[0]).body->nodes[0])->expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.type == E_TYPE::VALUE);
    REQUIRE(info->entity.value->metatype == Meta::CLASS);
    REQUIRE(*info->entity.value->type == ObjectType("List", {new ObjectType("String")}));
}

TEST_CASE("empty_dict_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {}::[Integer,String];return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    Node* expression = ((DeclarationNode*) (*(FunctionNode*) module.ast->nodes[0]).body->nodes[0])->expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.type == E_TYPE::VALUE);
    REQUIRE(info->entity.value->metatype == Meta::CLASS);
    REQUIRE(*info->entity.value->type == ObjectType("Dict", {new ObjectType("Integer"), new ObjectType("String")}));
}


TEST_CASE("dict_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {\"one\":1,\"two\":2};return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    Node* expression = ((DeclarationNode*) (*(FunctionNode*) module.ast->nodes[0]).body->nodes[0])->expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.type == E_TYPE::VALUE);
    REQUIRE(info->entity.value->metatype == Meta::CLASS);
    REQUIRE(*info->entity.value->type == ObjectType("Dict", {new ObjectType("String"), new ObjectType("Integer")}));
}

TEST_CASE("float_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9.5;return 0;}";

    Compiler c = analyze(code);
    Module& module = *c.root_package->units["tmp"].module;
    analyze_module_result(module, *c.top_package);
    Checker checker(c.top_package, c.root_package->units["tmp"].module);
    Node* expression = ((DeclarationNode*) (*(FunctionNode*) module.ast->nodes[0]).body->nodes[0])->expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.type == E_TYPE::VALUE);
    REQUIRE(info->entity.value->metatype == Meta::CLASS);
    REQUIRE(*info->entity.value->type == ObjectType("Float"));
}