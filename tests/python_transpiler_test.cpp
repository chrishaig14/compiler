#include "catch.hpp"
#include "../src/ast/expressions/include/UnaryOp.h"
#include "../src/compiler/analyze.h"
#include "../src/compiler/Compiler.h"
#include "../src/parser/Parser.h"
#include "../src/scanner/Scanner.h"
#include "../src/semantic/Checker.h"
#include "../src/semantic/errors/include/errors.h"
#include "../src/semantic/GlobalProcessor.h"
#include "../src/transpiler/PythonTranspiler.h"

TEST_CASE("python_transpile_string", "[checker]") {
    PythonTranspiler pt;
    // "Hello"
    PythonExpressionOutputCode poc = pt.transpile_string(sem::String("Hello"));
    REQUIRE(poc.code == "libcore.libcore.String(\"Hello\")");
}

TEST_CASE("python_transpile_integer", "[checker]") {
    PythonTranspiler pt;
    // 781
    PythonExpressionOutputCode poc = pt.transpile_integer(sem::Integer("781"));
    REQUIRE(poc.code == "libcore.libcore.Integer(781)");
}

TEST_CASE("python_transpile_boolean", "[checker]") {
    PythonTranspiler pt;
    // false
    PythonExpressionOutputCode poc = pt.transpile_boolean(sem::Bool(false));
    REQUIRE(poc.code == "libcore.libcore.Boolean(False)");
}

// TEST_CASE("python_transpile_object_method_call_no_args", "[checker]") {
//     PythonTranspiler pt;
//     // 19.to_str()
//     PythonOutputCode poc = pt.transpile_object_method_call(sem::ObjectMethodCallExp(std::make_unique<sem::Integer>("19"),
//                                                                                     Path("libcore.libcore.Integer"),
//                                                                                     "to_str",
//                                                                                     {}));
//     REQUIRE(poc.pre_code == "obj_0 = libcore.libcore.Integer(19)");
//     REQUIRE(poc.code == "libcore.libcore.Integer.to_str(obj_0)");
// }

// TEST_CASE("python_transpile_object_method_one_arg", "[checker]") {
//     PythonTranspiler pt;
//     std::vector<sem::UExp> args_vec;
//     args_vec.push_back(std::make_unique<sem::Integer>("23"));
//     // 19.to_str(23)
//     PythonOutputCode poc = pt.transpile_object_method_call(sem::ObjectMethodCallExp(std::make_unique<sem::Integer>("19"),
//                                                                                     Path("libcore.libcore.Integer"),
//                                                                                     "to_str",
//                                                                                     std::move(args_vec)));
//     REQUIRE(poc.pre_code == "obj_0 = libcore.libcore.Integer(19)\narg_1 = libcore.libcore.Integer(23)");
//     REQUIRE(poc.code == "libcore.libcore.Integer.to_str(obj_0, arg_1)");
// }
//
// TEST_CASE("python_transpile_object_method_with_args", "[checker]") {
//     PythonTranspiler pt;
//     std::vector<sem::UExp> args_vec;
//     args_vec.push_back(std::make_unique<sem::Integer>("23"));
//     args_vec.push_back(std::make_unique<sem::Integer>("87"));
//     //
//     PythonOutputCode poc = pt.transpile_object_method_call(sem::ObjectMethodCallExp(std::make_unique<sem::Integer>("19"),
//                                                                                     Path("libcore.libcore.Integer"),
//                                                                                     "to_str",
//                                                                                     std::move(args_vec)));
//     REQUIRE(poc.pre_code == "obj_0 = libcore.libcore.Integer(19)\narg_1 = libcore.libcore.Integer(23)\narg_2 = libcore.libcore.Integer(87)");
//     REQUIRE(poc.code == "libcore.libcore.Integer.to_str(obj_0, arg_1, arg_2)");
// }
//
// TEST_CASE("python_transpile_object_method_with_complex_args", "[checker]") {
//     PythonTranspiler pt;
//     std::vector<sem::UExp> args_vec;
//     std::vector<sem::UExp> complex_arg_vec;
//     complex_arg_vec.push_back(std::make_unique<sem::Integer>("45"));
//     args_vec.push_back(std::make_unique<sem::ObjectMethodCallExp>(std::make_unique<sem::Integer>("65"),
//                                                                Path("libcore.libcore.Integer"),
//                                                                "add",
//                                                                std::move(complex_arg_vec)));
//     args_vec.push_back(std::make_unique<sem::Integer>("87"));
//     // 19.mul(65.add(45),87)
//     PythonOutputCode poc = pt.transpile_object_method_call(sem::ObjectMethodCallExp(std::make_unique<sem::Integer>("19"),
//                                                                                     Path("libcore.libcore.Integer"),
//                                                                                     "mul",
//                                                                                     std::move(args_vec)));
//     REQUIRE(poc.pre_code == R"(obj_0 = libcore.libcore.Integer(19)
// obj_1 = libcore.libcore.Integer(65)
// arg_2 = libcore.libcore.Integer(45)
// arg_3 = libcore.libcore.Integer.add(obj_1, arg_2)
// arg_4 = libcore.libcore.Integer(87))");
//     REQUIRE(poc.code == "libcore.libcore.Integer.mul(obj_0, arg_3, arg_4)");
// }
//
TEST_CASE("python_transpile_const_function_call_with_complex_args", "[checker]") {
    PythonTranspiler pt;
    std::vector<sem::UExp> complex_arg_vec;
    complex_arg_vec.push_back(std::make_unique<sem::Integer>("45"));
    auto poc = pt.transpile_call_exp(sem::CallExp(std::make_unique<sem::ConstFunction>(Path("mymodule.myfunction")),
                                                  std::move(complex_arg_vec)));
    REQUIRE(poc.pre_code == R"(function_to_call_0 = mymodule.myfunction
arg_1 = libcore.libcore.Integer(45)
)");
    REQUIRE(poc.code == "function_to_call_0(arg_1)");
}
//
TEST_CASE("python_transpile_if", "[checker]") {
    PythonTranspiler pt;
    auto then = std::make_unique<sem::Block>();
    std::vector<sem::UExp> arg_vec;
    arg_vec.push_back(std::make_unique<sem::Integer>("78"));
    then->nodes.emplace_back(new sem::Assignment(std::make_unique<sem::Id>("x"), std::make_unique<sem::Integer>("99")));
    // if(8.gt(78)){x=99}
    PythonExpressionOutputCode poc = pt.transpile_if(sem::If(std::make_unique<sem::Bool>(true), std::move(then), {}, nullptr));
    REQUIRE(poc.code == R"(condition_0 = libcore.libcore.Boolean(True)
if condition_0:
    x = libcore.libcore.Integer(99))");
}

TEST_CASE("python_transpile_assignment", "[checker]") {
    PythonTranspiler pt;
    // x = 314
    PythonExpressionOutputCode poc = pt.transpile_assignment(sem::Assignment(std::make_unique<sem::Id>("x"),
                                                                             std::make_unique<sem::Integer>("314")));
    REQUIRE(poc.code == "x = libcore.libcore.Integer(314)");
}

// TEST_CASE("python_transpile_assignment_complex", "[checker]") {
//     PythonTranspiler pt;
//     std::vector<sem::UExp> arg_vec;
//     arg_vec.push_back(std::make_unique<sem::Integer>("69"));
//     // x = mymodule.myfunction(69)
//     PythonOutputCode poc = pt.transpile_assignment(sem::Assignment(std::make_unique<sem::Id>("x"),
//                                                                    std::make_unique<sem::ConstFunctionCallExp>(Path(
//                                                                                                                     "mymodule.myfunction"),
//                                                                                                             std::move(
//                                                                                                                     arg_vec))));
//     REQUIRE(poc.pre_code == "arg_0 = libcore.libcore.Integer(69)");
//     REQUIRE(poc.code == "x = mymodule.myfunction(arg_0)\n");
// }


TEST_CASE("python_transpile_while", "[checker]") {
    PythonTranspiler pt;
    auto then = std::make_unique<sem::Block>();
    then->nodes.emplace_back(new sem::Assignment(std::make_unique<sem::Id>("x"), std::make_unique<sem::Integer>("99")));
    // while(false){x=99}
    PythonExpressionOutputCode poc = pt.transpile_while(sem::While(std::make_unique<sem::Bool>(false), std::move(then)));
    REQUIRE(poc.code == R"(condition_0 = libcore.libcore.Boolean(False)
while condition_0:
    x = libcore.libcore.Integer(99)
    condition_0 = libcore.libcore.Boolean(False))");
}

TEST_CASE("python_transpile_function_def", "[checker]") {
    PythonTranspiler pt;
    auto body = std::make_unique<sem::Block>();
    body->nodes.emplace_back(new sem::Assignment(std::make_unique<sem::Id>("x"), std::make_unique<sem::Integer>("99")));
    // fun myfoo(a,b){x=99}
    PythonExpressionOutputCode poc = pt.transpile_function(sem::FunctionDef("myfoo", {"a", "b"}, std::move(body)));
    REQUIRE(poc.code == R"(def myfoo(a, b):
    x = libcore.libcore.Integer(99))");
}