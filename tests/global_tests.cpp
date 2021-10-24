#include "catch.hpp"
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <ast/expressions/include/UnaryOp.h>
#include <semantic/ModulePrechecker.h>
#include <simple_nodes/common/include/TypeObject.h>

const ast::ObjectType NO_TYPE(".None");

class ModulePrecheckerTest {
public:
    Module module;

    ModulePrecheckerTest(const std::string& code) : module(Path("main.foo"), "foo.xl", false) {
        Scanner scanner;
        scanner.load_text(code);
        std::vector<Token> tokens = scanner.scan_all();
        Parser parser("test", scanner.code_lines, tokens);
        module.ast = parser.parse_module();
        std::map<std::string, std::string> instances;
        ModulePrechecker gp(module, instances);
        gp.visit_root();
    }
};

TEST_CASE("global_main", "[parser]") {
    ModulePrecheckerTest ct("fun main()->Integer{return 0;}");
    Module& module = ct.module;
    REQUIRE(module.members.size() == 1);
    REQUIRE(module.members.count("main") == 1);
    REQUIRE(module.members["main"]->is_const_function());
    ConstFunction* const_function = &module.members["main"]->const_function();
    std::unique_ptr<sem::Type> u = std::make_unique<sem::TypeObject>("Integer");
    REQUIRE(const_function->const_function_ft == sem::TypeFunction({}, std::move(u)));
    REQUIRE(const_function->path.as_str() == "main.foo.main");
}

TEST_CASE("global_class", "[parser]") {
    ModulePrecheckerTest ct(R"(
class Foo {
    x:Integer
    y: String
    fun foo(w: String)->Integer{
        return 0
    }
    static fun static_foo(x: Boolean)->String{
        return "Hello"
    }
}
)");
    Module& module = ct.module;

    REQUIRE(module.members.size() == 1);
    REQUIRE(module.members.count("Foo") == 1);
    REQUIRE(module.members["Foo"]->is_klass());
    ConcreteClass* clazz = &module.members["Foo"]->klass();

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
    ModulePrecheckerTest ct(R"(
import moduleA
fun main()->Integer{
    return 0
}
class Foo {
    x: Integer
}
)");
    Module& module = ct.module;

    REQUIRE(module.members.size() == 2);
    REQUIRE(module.members.count("main") == 1);
    REQUIRE(module.members.count("Foo") == 1);

    REQUIRE(module.members["main"]->is_const_function());
    ConstFunction* const_function = &module.members["main"]->const_function();
    REQUIRE(const_function->const_function_ft == sem::TypeFunction({}, std::make_unique<sem::TypeObject>("Integer")));
    REQUIRE(const_function->path.as_str() == "main.foo.main");


    ConcreteClass* clazz = &module.members["Foo"]->klass();

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