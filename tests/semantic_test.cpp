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
#include "../src/simple_nodes/TypeObject.h"
#include "../src/simple_nodes/TypeFunction.h"
#include "../src/semantic/errors/ErrorClassNoMember.h"
#include "../src/semantic/errors/ErrorNoMemberSuggestions.h"
#include "../src/semantic/errors/ErrorClassNoMethodForOp.h"
#include "../src/semantic/errors/ErrorObjectNoSpecialMethod.h"
#include "../src/semantic/errors/ErrorEnumNoValue.h"

static const ast::ObjectType NO_TYPE(".None");

static const TextPosition& _POS = {1, 1};

std::unique_ptr<Compiler> c_analyze(std::string code) {
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

TEST_CASE("semantic_output_basic_function", "[checker]") {
    std::string code = "fun foo()->Integer{return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_function(module.ast->functions[0]);
    REQUIRE(not checker.error_reporter.failed);
    std::unique_ptr<sem::Block> b = std::make_unique<sem::Block>();
    b->nodes.emplace_back(std::make_unique<sem::Return>(std::make_unique<sem::Integer>("0")));
    auto exp = sem::FunctionDef("test.tmp.foo", VectorOfStrings{}, std::move(b));
    REQUIRE(*info->snode == exp);
}

TEST_CASE("semantic_output_basic_declaration", "[checker]") {
    std::string code = "fun main()->Integer{var x = 9;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_declaration((ast::Declaration&) *module.ast->functions[0].get().body->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
    auto exp = sem::Declaration("x", std::make_unique<sem::Integer>("9"));
    REQUIRE(*info->snode == exp);
}

TEST_CASE("semantic_output_list", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,1];return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_declaration((ast::Declaration&) *module.ast->functions[0].get().body->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
    std::vector<USNode> e;
    e.push_back(std::make_unique<sem::Integer>("4"));
    e.push_back(std::make_unique<sem::Integer>("1"));
    auto exp = sem::Declaration("x", std::make_unique<sem::List>(std::move(e)));
    REQUIRE(*info->snode == exp);
}


TEST_CASE("semantic_output_empty_dict", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {}::[Integer,String];return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_declaration((ast::Declaration&) *module.ast->functions[0].get().body->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
    std::vector<std::pair<USNode, USNode>> e;
    auto exp = sem::Declaration("x", std::make_unique<sem::Dict>(std::move(e)));
    REQUIRE(*info->snode == exp);
}

TEST_CASE("semantic_output_dict", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",9:\"nine\"};return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_declaration((ast::Declaration&) *module.ast->functions[0].get().body->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
    std::vector<std::pair<USNode, USNode>> e;
    e.emplace_back(std::make_unique<sem::Integer>("7"), std::make_unique<sem::String>("seven"));
    e.emplace_back(std::make_unique<sem::Integer>("9"), std::make_unique<sem::String>("nine"));
    auto exp = sem::Declaration("x", std::make_unique<sem::Dict>(std::move(e)));
    REQUIRE(*info->snode == exp);
}

TEST_CASE("semantic_output_int_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_declaration((ast::Declaration&) *module.ast->functions[0].get().body->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
    auto exp = sem::Declaration("x", std::make_unique<sem::Integer>("9"));
    REQUIRE(*info->snode == exp);
}

TEST_CASE("semantic_output_bool_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = false;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_declaration((ast::Declaration&) *module.ast->functions[0].get().body->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
    auto exp = sem::Declaration("x", std::make_unique<sem::Bool>(false));
    REQUIRE(*info->snode == exp);
}

TEST_CASE("semantic_output_empty_list_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = []::String;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_declaration((ast::Declaration&) *module.ast->functions[0].get().body->nodes[0]);
    REQUIRE(not checker.error_reporter.failed);
    std::vector<USNode> e;
    auto exp = sem::Declaration("x", std::make_unique<sem::List>(std::move(e)));
    REQUIRE(*info->snode == exp);
}

TEST_CASE("semantic_output_object_member", "[checker]") {
    std::string code = "class Foo {bar: Integer;} fun foo(f:Foo)->Integer{var x = f.bar;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_function(module.ast->functions[0]);
    REQUIRE(not checker.error_reporter.failed);
    std::vector<USNode> e;
    auto exp = sem::Declaration("x",
                                std::make_unique<sem::ObjectMember>(std::make_unique<sem::Id>("f"),
                                                                    Path("test.tmp.Foo"),
                                                                    "bar"));
    REQUIRE(*(*(std::unique_ptr<sem::FunctionDef>&) info->snode).body->nodes[0] == exp);
}

TEST_CASE("semantic_output_object_method_call", "[checker]") {
    std::string code = "class Foo {bar: Integer;fun get_foo()->Integer{return 0;}} fun foo(f:Foo)->Integer{var x = f.get_foo();return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_function(module.ast->functions[0]);
    REQUIRE(not checker.error_reporter.failed);
    std::vector<USNode> e;
    auto exp = sem::Declaration("x",
                                std::make_unique<sem::ObjectMethodCall>(std::make_unique<sem::Id>("f"),
                                                                        Path("test.tmp.Foo"),
                                                                        "get_foo",
                                                                        std::vector<USNode>()));
    REQUIRE(*(*(std::unique_ptr<sem::FunctionDef>&) info->snode).body->nodes[0] == exp);
}

TEST_CASE("semantic_output_object_method", "[checker]") {
    std::string code = "class Foo {bar: Integer;fun get_foo()->Integer{return 0;}} fun foo(f:Foo)->Integer{var x = f.get_foo;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    USemanticInfo info = checker.visit_function(module.ast->functions[0]);
    REQUIRE(not checker.error_reporter.failed);
    std::vector<USNode> e;
    auto exp = sem::Declaration("x",
                                std::make_unique<sem::ObjectMethod>(std::make_unique<sem::Id>("f"),
                                                                    Path("test.tmp.Foo"),
                                                                    "get_foo"));
    REQUIRE(*(*(std::unique_ptr<sem::FunctionDef>&) info->snode).body->nodes[0] == exp);
}

TEST_CASE("semantic_output_assign_const_function", "[checker]") {
    std::string code = "fun bar()->Integer{return 0;} fun foo()->Integer{var x = bar;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    resolve_module_imports(module, c.top_package);
    for (auto& e: module.members) {
        std::cout << e.first << std::endl;
    }
    Checker checker(c.top_package, module);
    checker.init();
    USemanticInfo info = checker.visit_function(module.ast->functions[1]);
    REQUIRE(not checker.error_reporter.failed);
    std::vector<USNode> e;
    auto exp = sem::Declaration("x", std::make_unique<sem::ConstFunction>(Path("test.tmp.bar")));
    REQUIRE(*(*(std::unique_ptr<sem::FunctionDef>&) info->snode).body->nodes[0] == exp);
}

TEST_CASE("semantic_output_const_function_call", "[checker]") {
    std::string code = "fun bar()->Integer{return 0;} fun foo()->Integer{var x = bar();return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    resolve_module_imports(module, c.top_package);
    for (auto& e: module.members) {
        std::cout << e.first << std::endl;
    }
    Checker checker(c.top_package, module);
    checker.init();
    USemanticInfo info = checker.visit_function(module.ast->functions[1]);
    REQUIRE(not checker.error_reporter.failed);
    std::vector<USNode> e;
    auto exp = sem::Declaration("x",
                                std::make_unique<sem::ConstFunctionCall>(Path("test.tmp.bar"), std::vector<USNode>{}));
    REQUIRE(*(*(std::unique_ptr<sem::FunctionDef>&) info->snode).body->nodes[0] == exp);
}

TEST_CASE("semantic_output_while", "[checker]") {
    std::string code = "fun foo()->Integer{while true {var x = 1;} return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    resolve_module_imports(module, c.top_package);
    for (auto& e: module.members) {
        std::cout << e.first << std::endl;
    }
    Checker checker(c.top_package, module);
    checker.init();
    USemanticInfo info = checker.visit_function(module.ast->functions[0]);
    REQUIRE(not checker.error_reporter.failed);
    std::vector<USNode> e;
    auto block = std::make_unique<sem::Block>();
    block->nodes.emplace_back(new sem::Declaration("x", std::make_unique<sem::Integer>("1")));
    auto exp = sem::While(std::make_unique<sem::Bool>(true), std::move(block));
    REQUIRE(*(*(std::unique_ptr<sem::FunctionDef>&) info->snode).body->nodes[0] == exp);
}


TEST_CASE("semantic_output_float_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9.5;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    ast::Node& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().type == E_TYPE::VALUE);
    REQUIRE(((Value&) (info->entity.get())).metatype == Meta::CLASS);
    REQUIRE(((Value&) (info->entity.get())).type == sem::TypeObject("Float"));
}

// TEST_CASE("semantic_output_none_literal", "[checker]") {
//     std::string code = "fun foo()->Integer{var x = none;return 0;}";
//
//     std::unique_ptr<Compiler> cp =  c_analyze(code);Compiler& c = *cp;
//     Module& module = *c.root_package.units["tmp"].module;
//     analyze_module_result(module, c.top_package);
//     Checker checker(c.top_package,module);
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

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    CHECK(!checker.error_reporter.failed);
    CHECK(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_binop", "[checker]") {
    std::string code = "fun bar()->Integer{var x = 2 + 5;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    checker.init();

    ast::Node& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
    CHECK(info->entity.get().type == E_TYPE::VALUE);
    Value& entity_value = (Value&) (info->entity.get());
    CHECK(entity_value.metatype == Meta::CLASS);
    CHECK(entity_value.type == sem::TypeObject("Integer"));
}

TEST_CASE("semantic_output_boolop", "[checker]") {
    std::string code = "fun bar()->Integer{var x = 2 < 5;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    checker.init();

    ast::Node& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;

    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
    CHECK(info->entity.get().type == E_TYPE::VALUE);
    Value& entity_value = (Value&) (info->entity.get());
    CHECK(entity_value.metatype == Meta::CLASS);
    CHECK(entity_value.type ==sem::TypeObject("Boolean"));
}

TEST_CASE("semantic_output_subscript", "[checker]") {
    std::string code = "fun bar()->Integer{var x = [1,3,4][2] ;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    checker.init();

    ast::Node& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;

    USemanticInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_call_no_args", "[checker]") {
    std::string code = "fun bar()->Integer{return 0;}\nfun foo()->Integer{var x : Integer = bar();return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_call_args", "[checker]") {
    std::string code = "fun bar(a: Integer, b: String)->Integer{return 0;}\nfun foo()->Integer{var x : Integer = bar(8, \"Hello\");return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_union_ok_1", "[checker]") {
    std::string code = "fun foo()->Integer{var x : Union[Integer, String] = 3;return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_union_ok_2", "[checker]") {
    std::string code = "fun foo()->Integer{var x : Union[Integer, String] = \"String\";return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_if", "[checker]") {
    std::string code = "fun foo()->Integer{if true {var x = 1;}return 0;}";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    checker.init();
    checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());
}

TEST_CASE("semantic_output_enum_def", "[checker]") {
    std::string code = "enum Foo {a, c}\n";

    std::unique_ptr<Compiler> cp = c_analyze(code);
    Compiler& c = *cp;
    Module& module = *c.root_package.units["tmp"].module;
    resolve_module_imports(module, c.top_package);
    Checker checker(c.top_package, module);
    checker.init();
    USemanticInfoBlock info = checker.visit_root(*module.ast);

    REQUIRE(!checker.error_reporter.failed);
    REQUIRE(checker.error_reporter.errors.empty());

    REQUIRE(*((info->snode)->nodes[0]) == sem::EnumDef("test.tmp.Foo", {"a", "c"}));

}