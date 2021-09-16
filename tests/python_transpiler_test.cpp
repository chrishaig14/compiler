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
    // "Hello"
    PythonOutputCode poc = pt.transpile_string(sem::String("Hello"));
    REQUIRE(poc.code == "String(\"Hello\")");
}

TEST_CASE("python_transpile_integer", "[checker]") {
    PythonTranspiler pt;
    // 781
    PythonOutputCode poc = pt.transpile_integer(sem::Integer("781"));
    REQUIRE(poc.code == "781");
}

TEST_CASE("python_transpile_boolean", "[checker]") {
    PythonTranspiler pt;
    // false
    PythonOutputCode poc = pt.transpile_boolean(sem::Bool(false));
    REQUIRE(poc.code == "False");
}

TEST_CASE("python_transpile_object_method_call_no_args", "[checker]") {
    PythonTranspiler pt;
    // 19.to_str()
    PythonOutputCode poc = pt.transpile_object_method_call(sem::ObjectMethodCall(std::make_unique<sem::Integer>("19"),
                                                                                 Path("core.core.Integer"),
                                                                                 "to_str",
                                                                                 {}));
    REQUIRE(poc.pre_code == "object = 19");
    REQUIRE(poc.code == "core.core.Integer.to_str(object)");
}

TEST_CASE("python_transpile_object_method_one_arg", "[checker]") {
    PythonTranspiler pt;
    std::vector<USNode> args_vec;
    args_vec.push_back(std::make_unique<sem::Integer>("23"));
    // 19.to_str(23)
    PythonOutputCode poc = pt.transpile_object_method_call(sem::ObjectMethodCall(std::make_unique<sem::Integer>("19"),
                                                                                 Path("core.core.Integer"),
                                                                                 "to_str",
                                                                                 std::move(args_vec)));
    REQUIRE(poc.pre_code == "object = 19\narg0 = 23");
    REQUIRE(poc.code == "core.core.Integer.to_str(object, arg0)");
}

TEST_CASE("python_transpile_object_method_with_args", "[checker]") {
    PythonTranspiler pt;
    std::vector<USNode> args_vec;
    args_vec.push_back(std::make_unique<sem::Integer>("23"));
    args_vec.push_back(std::make_unique<sem::Integer>("87"));
    //
    PythonOutputCode poc = pt.transpile_object_method_call(sem::ObjectMethodCall(std::make_unique<sem::Integer>("19"),
                                                                                 Path("core.core.Integer"),
                                                                                 "to_str",
                                                                                 std::move(args_vec)));
    REQUIRE(poc.pre_code == "object = 19\narg0 = 23\narg1 = 87");
    REQUIRE(poc.code == "core.core.Integer.to_str(object, arg0, arg1)");
}

TEST_CASE("python_transpile_object_method_with_complex_args", "[checker]") {
    PythonTranspiler pt;
    std::vector<USNode> args_vec;
    std::vector<USNode> complex_arg_vec;
    complex_arg_vec.push_back(std::make_unique<sem::Integer>("45"));
    args_vec.push_back(std::make_unique<sem::ObjectMethodCall>(std::make_unique<sem::Integer>("65"),
                                                               Path("core.core.Integer"),
                                                               "add",
                                                               std::move(complex_arg_vec)));
    args_vec.push_back(std::make_unique<sem::Integer>("87"));
    PythonOutputCode poc = pt.transpile_object_method_call(sem::ObjectMethodCall(std::make_unique<sem::Integer>("19"),
                                                                                 Path("core.core.Integer"),
                                                                                 "mul",
                                                                                 std::move(args_vec)));
    REQUIRE(poc.pre_code == R"(object = 19
object = 65
arg0 = 45
arg0 = core.core.Integer.add(object, arg0)
arg1 = 87)");
    REQUIRE(poc.code == "core.core.Integer.mul(object, arg0, arg1)");
}

TEST_CASE("python_transpile_const_function_call_with_complex_args", "[checker]") {
    PythonTranspiler pt;
    std::vector<USNode> args_vec;
    std::vector<USNode> complex_arg_vec;
    complex_arg_vec.push_back(std::make_unique<sem::Integer>("45"));
    args_vec.push_back(std::make_unique<sem::ObjectMethodCall>(std::make_unique<sem::Integer>("65"),
                                                               Path("core.core.Integer"),
                                                               "add",
                                                               std::move(complex_arg_vec)));
    args_vec.push_back(std::make_unique<sem::Integer>("87"));
    PythonOutputCode poc = pt.transpile_const_function_call(sem::ConstFunctionCall(Path("mymodule.myfunction"),
                                                                                   std::move(args_vec)));
    REQUIRE(poc.pre_code == R"(object = 65
arg0 = 45
arg0 = core.core.Integer.add(object, arg0)
arg1 = 87)");
    REQUIRE(poc.code == "mymodule.myfunction(arg0, arg1)");
}

TEST_CASE("python_transpile_if", "[checker]") {
    PythonTranspiler pt;
    auto then = std::make_unique<sem::Block>();
    std::vector<USNode> arg_vec;
    arg_vec.push_back(std::make_unique<sem::Integer>("78"));
    then->nodes.emplace_back(new sem::Assignment(std::make_unique<sem::Id>("x"), std::make_unique<sem::Integer>("99")));
    // if(8.gt(78)){x=99}
    PythonOutputCode poc = pt.transpile_if(sem::IfSNode(std::make_unique<sem::ObjectMethodCall>(std::make_unique<sem::Integer>(
            "8"), Path("core.core.Integer"), "gt", std::move(arg_vec)), std::move(then), {}, nullptr));
    REQUIRE(poc.pre_code == R"(object = 8
arg0 = 78
condition = core.core.Integer.gt(object, arg0))");
    REQUIRE(poc.code == R"(if condition:
    x = 99
)");
}

TEST_CASE("python_transpile_assignment", "[checker]") {
    PythonTranspiler pt;
    // x = 314
    PythonOutputCode poc = pt.transpile_assignment(sem::Assignment(std::make_unique<sem::Id>("x"),
                                                                   std::make_unique<sem::Integer>("314")));
    REQUIRE(poc.code == "x = 314\n");
}

TEST_CASE("python_transpile_assignment_complex", "[checker]") {
    PythonTranspiler pt;
    std::vector<USNode> arg_vec;
    arg_vec.push_back(std::make_unique<sem::Integer>("69"));
    // x = mymodule.myfunction(69)
    PythonOutputCode poc = pt.transpile_assignment(sem::Assignment(std::make_unique<sem::Id>("x"),
                                                                   std::make_unique<sem::ConstFunctionCall>(Path(
                                                                                                                    "mymodule.myfunction"),
                                                                                                            std::move(
                                                                                                                    arg_vec))));
    REQUIRE(poc.pre_code == "arg0 = 69");
    REQUIRE(poc.code == "x = mymodule.myfunction(arg0)\n");
}


TEST_CASE("python_transpile_while", "[checker]") {
    PythonTranspiler pt;
    auto then = std::make_unique<sem::Block>();
    then->nodes.emplace_back(new sem::Assignment(std::make_unique<sem::Id>("x"), std::make_unique<sem::Integer>("99")));
    // while(false){x=99}
    PythonOutputCode poc = pt.transpile_while(sem::While(std::make_unique<sem::Bool>(false), std::move(then)));
    REQUIRE(poc.code == "while (False):\n\tx = 99\n");
}

TEST_CASE("python_transpile_function_def", "[checker]") {
    PythonTranspiler pt;
    auto body = std::make_unique<sem::Block>();
    body->nodes.emplace_back(new sem::Assignment(std::make_unique<sem::Id>("x"), std::make_unique<sem::Integer>("99")));
    // fun myfoo(a,b){x=99}
    PythonOutputCode poc = pt.transpile_function(sem::FunctionDef("myfoo", {"a", "b"}, std::move(body)));
    REQUIRE(poc.code == "def myfoo(a, b):\n\tx = 99\n");
}