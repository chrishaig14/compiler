#include "catch.hpp"
#include "../src/scanner/Scanner.h"
#include "../src/parser/Parser.h"
#include "../src/nodes/UnaryOpNode.h"
#include "../src/semantic/GlobalProcessor.h"
#include "../src/semantic/Checker.h"
#include "../src/compiler/Compiler.h"
#include "../src/compiler/analyze.h"
#include "../src/semantic/errors/ErrorTypeMismatch.h"

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