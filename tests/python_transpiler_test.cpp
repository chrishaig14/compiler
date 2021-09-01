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
#include "../src/transpiler/PythonTranspiler.h"

TEST_CASE("python_transpile_string", "[checker]") {
    PythonTranspiler pt;
    PythonOutputCode poc = pt.transpile_string(sem::String("Hello"));
    REQUIRE(poc.code == "\"Hello\"");
}

TEST_CASE("python_transpile_integer", "[checker]") {
    PythonTranspiler pt;
    PythonOutputCode poc = pt.transpile_integer(sem::Integer("781"));
    REQUIRE(poc.code == "781");
}

TEST_CASE("python_transpile_boolean", "[checker]") {
    PythonTranspiler pt;
    PythonOutputCode poc = pt.transpile_boolean(sem::Bool(false));
    REQUIRE(poc.code == "False");
}

TEST_CASE("python_transpile_if", "[checker]") {
    PythonTranspiler pt;
    sem::Block* then = new sem::Block();
    then->nodes = {new sem::Assignment(std::make_unique<sem::Id>("x"), std::make_unique<sem::Integer>("99"))};
    PythonOutputCode poc = pt.transpile_if(sem::IfSNode(new sem::Bool(false), then, {}, nullptr));
    REQUIRE(poc.code == "if (False):\n\tx = 99\n");
}

TEST_CASE("python_transpile_assignment", "[checker]") {
    PythonTranspiler pt;
    PythonOutputCode poc = pt.transpile_assignment(sem::Assignment(std::make_unique<sem::Id>("x"),
                                                                   std::make_unique<sem::Integer>("314")));
    REQUIRE(poc.code == "x = 314\n");
}

TEST_CASE("python_transpile_while", "[checker]") {
    PythonTranspiler pt;
    sem::Block* then = new sem::Block();
    then->nodes = {new sem::Assignment(std::make_unique<sem::Id>("x"), std::make_unique<sem::Integer>("99"))};
    PythonOutputCode poc = pt.transpile_while(sem::While(std::make_unique<sem::Bool>(false), then));
    REQUIRE(poc.code == "while (False):\n\tx = 99\n");
}

TEST_CASE("python_transpile_function_def", "[checker]") {
    PythonTranspiler pt;
    sem::Block* then = new sem::Block();
    then->nodes = {new sem::Assignment(std::make_unique<sem::Id>("x"), std::make_unique<sem::Integer>("99"))};
    auto* body = new sem::Block();
    body->nodes = {new sem::Assignment(std::make_unique<sem::Id>("x"), std::make_unique<sem::Integer>("99"))};
    PythonOutputCode poc = pt.transpile_function(sem::FunctionDef("myfoo", {"a", "b"}, body));
    REQUIRE(poc.code == "def myfoo(a, b):\n\tx = 99\n");
}