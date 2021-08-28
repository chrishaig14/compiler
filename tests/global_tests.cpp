#include "catch.hpp"
#include "../src/scanner/Scanner.h"
#include "../src/parser/Parser.h"
#include "../src/ast/UnaryOp.h"
#include "../src/semantic/GlobalProcessor.h"

const ObjectType NO_TYPE(".None");

TEST_CASE("global_main", "[parser]") {
    Scanner scanner;
    std::string code = "fun main()->Integer{return 0;}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";
    Module module(Path("main.foo"), "foo.xl", ".", false, "main.h");
    module.ast = parser.parse_program();
    GlobalProcessor gp(module);
    gp.visit_root();

    REQUIRE(module.flirpins.size() == 1);
    REQUIRE(module.flirpins.count("main") == 1);
    REQUIRE(module.flirpins["main"].type == F_TYPE::CONST_FUNCTION);
    ConstFunction* const_function = module.flirpins["main"].const_function;

    REQUIRE(*const_function->ft == FunctionType({}, new ObjectType("Integer")));
    REQUIRE(const_function->implicit == nullptr);
    REQUIRE(const_function->path.as_str() == "main.foo.main");
}

TEST_CASE("global_class", "[parser]") {
    Scanner scanner;
    std::string code = "class Foo {x:Integer;y: String; fun foo(w: String)->Integer{return 0;} static fun static_foo(x: Boolean)->String{return \"Hello\";}}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";
    Module module(Path("main.foo"), "foo.xl", ".", false, "main.h");
    module.ast = parser.parse_program();
    GlobalProcessor gp(module);
    gp.visit_root();

    REQUIRE(module.flirpins.size() == 1);
    REQUIRE(module.flirpins.count("Foo") == 1);
    REQUIRE(module.flirpins["Foo"].type == F_TYPE::CLASS);
    Class* clazz = module.flirpins["Foo"].clazz;

    REQUIRE(clazz->class_name == "Foo");
    REQUIRE(clazz->path.as_str() == "main.foo.Foo");
    REQUIRE(clazz->type_params.empty());

    REQUIRE(clazz->members.size() == 2);
    REQUIRE(clazz->members.count("x") == 1);
    REQUIRE(*clazz->members["x"] == ObjectType("Integer"));
    REQUIRE(clazz->members.count("y") == 1);
    REQUIRE(*clazz->members["y"] == ObjectType("String"));

    REQUIRE(clazz->member_entities.size() == 2);
    REQUIRE(clazz->member_entities.count("x") == 1);
    REQUIRE(clazz->member_entities.count("y") == 1);
    // REQUIRE(clazz->member_entities["x"]==z);

    REQUIRE(clazz->member_names.size() == 2);
    REQUIRE(clazz->member_names[0] == "x");
    REQUIRE(clazz->member_names[1] == "y");

    REQUIRE(clazz->member_types.size() == 2);
    REQUIRE(*clazz->member_types[0] == ObjectType("Integer"));
    REQUIRE(*clazz->member_types[1] == ObjectType("String"));

    REQUIRE(clazz->static_members.empty());

    REQUIRE(clazz->methods.size() == 1);
    REQUIRE(clazz->static_methods.size() == 1);
}

TEST_CASE("global_multiple", "[parser]") {
    Scanner scanner;
    std::string code = "fun main()->Integer{return 0;}\nclass Foo {x: Integer;}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";
    Module module(Path("main.foo"), "foo.xl", ".", false, "main.h");
    module.ast = parser.parse_program();
    GlobalProcessor gp(module);
    gp.visit_root();

    REQUIRE(module.flirpins.size() == 2);
    REQUIRE(module.flirpins.count("main") == 1);
    REQUIRE(module.flirpins.count("Foo") == 1);

    REQUIRE(module.flirpins["main"].type == F_TYPE::CONST_FUNCTION);
    ConstFunction* const_function = module.flirpins["main"].const_function;
    REQUIRE(*const_function->ft == FunctionType({}, new ObjectType("Integer")));
    REQUIRE(const_function->implicit == nullptr);
    REQUIRE(const_function->path.as_str() == "main.foo.main");


    Class* clazz = module.flirpins["Foo"].clazz;

    REQUIRE(clazz->class_name == "Foo");
    REQUIRE(clazz->path.as_str() == "main.foo.Foo");
    REQUIRE(clazz->type_params.empty());

    REQUIRE(clazz->members.size() == 1);
    REQUIRE(clazz->members.count("x") == 1);
    REQUIRE(*clazz->members["x"] == ObjectType("Integer"));

    REQUIRE(clazz->member_entities.size() == 1);
    REQUIRE(clazz->member_entities.count("x") == 1);
    // REQUIRE(clazz->member_entities["x"]==z);

    REQUIRE(clazz->member_names.size() == 1);
    REQUIRE(clazz->member_names[0] == "x");

    REQUIRE(clazz->member_types.size() == 1);
    REQUIRE(*clazz->member_types[0] == ObjectType("Integer"));

    REQUIRE(clazz->static_members.empty());

    REQUIRE(clazz->methods.empty());
    REQUIRE(clazz->static_methods.empty());
}