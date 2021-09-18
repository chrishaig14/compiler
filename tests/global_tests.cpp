#include "catch.hpp"
#include "../src/scanner/Scanner.h"
#include "../src/parser/Parser.h"
#include "../src/ast/UnaryOp.h"
#include "../src/semantic/GlobalProcessor.h"
#include "../src/simple_nodes/common/TypeObject.h"

const ast::ObjectType NO_TYPE(".None");

TEST_CASE("global_main", "[parser]") {
    Scanner scanner;
    std::string code = "fun main()->Integer{return 0;}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    Module module(Path("main.foo"), "foo.xl", false);
    module.ast = parser.parse_module();
    GlobalProcessor gp(module);
    gp.visit_root();

    REQUIRE(module.members.size() == 1);
    REQUIRE(module.members.count("main") == 1);
    REQUIRE(module.members["main"].type == ModuleMemberType::CONST_FUNCTION);
    ConstFunction* const_function = module.members["main"].const_function;
    // sem::Type* p = (sem::TypeObject*) nullptr;
    std::unique_ptr<sem::Type> u = std::make_unique<sem::TypeObject>("Integer");
    REQUIRE(const_function->const_function_ft == sem::TypeFunction({}, std::move(u)));
    REQUIRE(const_function->implicit == nullptr);
    REQUIRE(const_function->path.as_str() == "main.foo.main");
}

TEST_CASE("global_class", "[parser]") {
    Scanner scanner;
    std::string code = "class Foo {x:Integer;y: String; fun foo(w: String)->Integer{return 0;} static fun static_foo(x: Boolean)->String{return \"Hello\";}}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    Module module(Path("main.foo"), "foo.xl", false);
    module.ast = parser.parse_module();
    GlobalProcessor gp(module);
    gp.visit_root();

    REQUIRE(module.members.size() == 1);
    REQUIRE(module.members.count("Foo") == 1);
    REQUIRE(module.members["Foo"].type == ModuleMemberType::CLASS);
    Class* clazz = module.members["Foo"].clazz;

    REQUIRE(clazz->class_name == "Foo");
    REQUIRE(clazz->path.as_str() == "main.foo.Foo");
    REQUIRE(clazz->type_params.empty());

    REQUIRE(clazz->members.size() == 2);
    REQUIRE(clazz->members.count("x") == 1);
    REQUIRE(*clazz->members["x"] == ast::ObjectType("Integer"));
    REQUIRE(clazz->members.count("y") == 1);
    REQUIRE(*clazz->members["y"] == ast::ObjectType("String"));

    REQUIRE(clazz->member_entities.size() == 2);
    REQUIRE(clazz->member_entities.count("x") == 1);
    REQUIRE(clazz->member_entities.count("y") == 1);
    // REQUIRE(clazz->member_entities["x"]==z);

    REQUIRE(clazz->member_names.size() == 2);
    REQUIRE(clazz->member_names[0] == "x");
    REQUIRE(clazz->member_names[1] == "y");

    REQUIRE(clazz->member_types.size() == 2);
    REQUIRE(*clazz->member_types[0] == ast::ObjectType("Integer"));
    REQUIRE(*clazz->member_types[1] == ast::ObjectType("String"));

    REQUIRE(clazz->static_members.empty());

    REQUIRE(clazz->methods.size() == 1);
    REQUIRE(clazz->static_methods.size() == 1);
}

TEST_CASE("global_multiple", "[parser]") {
    Scanner scanner;
    std::string code = "import moduleA; fun main()->Integer{return 0;}\nclass Foo {x: Integer;}\n";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    Module module(Path("main.foo"), "foo.xl", false);
    module.ast = parser.parse_module();
    GlobalProcessor gp(module);
    gp.visit_root();

    REQUIRE(module.members.size() == 2);
    REQUIRE(module.members.count("main") == 1);
    REQUIRE(module.members.count("Foo") == 1);

    REQUIRE(module.members["main"].type == ModuleMemberType::CONST_FUNCTION);
    ConstFunction* const_function = module.members["main"].const_function;
    REQUIRE(const_function->const_function_ft == sem::TypeFunction({}, std::make_unique<sem::TypeObject>("Integer")));
    REQUIRE(const_function->implicit == nullptr);
    REQUIRE(const_function->path.as_str() == "main.foo.main");


    Class* clazz = module.members["Foo"].clazz;

    REQUIRE(clazz->class_name == "Foo");
    REQUIRE(clazz->path.as_str() == "main.foo.Foo");
    REQUIRE(clazz->type_params.empty());

    REQUIRE(clazz->members.size() == 1);
    REQUIRE(clazz->members.count("x") == 1);
    REQUIRE(*clazz->members["x"] == ast::ObjectType("Integer"));

    REQUIRE(clazz->member_entities.size() == 1);
    REQUIRE(clazz->member_entities.count("x") == 1);
    // REQUIRE(clazz->member_entities["x"]==z);

    REQUIRE(clazz->member_names.size() == 1);
    REQUIRE(clazz->member_names[0] == "x");

    REQUIRE(clazz->member_types.size() == 1);
    REQUIRE(*clazz->member_types[0] == ast::ObjectType("Integer"));

    REQUIRE(clazz->static_members.empty());

    REQUIRE(clazz->methods.empty());
    REQUIRE(clazz->static_methods.empty());
}