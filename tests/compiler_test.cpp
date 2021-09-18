#include "catch.hpp"
#include "../src/scanner/Scanner.h"
#include "../src/parser/Parser.h"
#include "../src/ast/UnaryOp.h"
#include "../src/semantic/GlobalProcessor.h"
#include "../src/simple_nodes/TypeObject.h"
#include "../src/compiler/Compiler.h"

const ast::ObjectType NO_TYPE(".None");

void write_file(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    file << content;
    file.flush();
}

const std::string IN_DIR = "/home/chris/CLionProjects/compiler/tmp_in";
const std::string OUT_DIR = "tmp_out";
const std::string OUT_NAME = "test_program";
const std::string LIB_PATH = "/home/chris/CLionProjects/compiler/lib";
const bool IS_LIB = false;
const std::string VERSION = "1.0.0";

void setup_dirs() {
    std::string cmd = "rm -rf " + IN_DIR;
    system(cmd.c_str());
    int r = mkdir(IN_DIR.c_str(), 0700);
    REQUIRE(r == 0);
}

void cleanup_dirs() {
    std::string cmd = "rm -rf " + IN_DIR;
    system(cmd.c_str());
    cmd = "rm -rf " + OUT_DIR;
    system(cmd.c_str());
}

void make_dir(const std::string& path) {
    std::string cmd = "mkdir -p " + path;
    system(cmd.c_str());
}

TEST_CASE("load_project_test", "[compiler]") {
    setup_dirs();
    write_file(path_join(IN_DIR, "requirements.txt"), "core==1.0.0");
    Compiler c(IN_DIR, OUT_DIR, OUT_NAME, LIB_PATH, IS_LIB, VERSION);

    SECTION("only one module") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");

        load_package(c.root_package, 1);

        REQUIRE(c.top_package.units.count(OUT_NAME) == 1);
        Unit u = c.top_package.units[OUT_NAME];
        REQUIRE(u.type == U_TYPE::PACKAGE);

        REQUIRE(u.package->units.size() == 1);

        REQUIRE(u.package->units.count("main") == 1);
        Unit m = u.package->units["main"];
        REQUIRE(m.type == U_TYPE::MODULE);
    }

    SECTION("multiple modules") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "module.xl"), "fun bar()->Integer{return 0;}");

        load_package(c.root_package, 1);

        REQUIRE(c.top_package.units.count(OUT_NAME) == 1);
        Unit u = c.top_package.units[OUT_NAME];
        REQUIRE(u.type == U_TYPE::PACKAGE);

        REQUIRE(u.package->units.size() == 2);

        REQUIRE(u.package->units.count("main") == 1);
        Unit main_module = u.package->units["main"];
        REQUIRE(main_module.type == U_TYPE::MODULE);

        REQUIRE(u.package->units.count("module") == 1);
        Unit module_module = u.package->units["module"];
        REQUIRE(module_module.type == U_TYPE::MODULE);
    }

    SECTION("one subpackage") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "module.xl"), "fun bar()->Integer{return 0;}");

        const std::string& SUBPACKAGE_PATH = path_join(IN_DIR, "subpackage");
        make_dir(SUBPACKAGE_PATH);
        write_file(path_join(SUBPACKAGE_PATH, "moduleA.xl"), "fun baz()->Integer{return 0;}");
        write_file(path_join(SUBPACKAGE_PATH, "moduleB.xl"), "fun foobar()->Integer{return 0;}");

        load_package(c.root_package, 1);

        REQUIRE(c.top_package.units.count(OUT_NAME) == 1);
        Unit u = c.top_package.units[OUT_NAME];
        REQUIRE(u.type == U_TYPE::PACKAGE);

        REQUIRE(u.package->units.size() == 3);

        REQUIRE(u.package->units.count("main") == 1);
        Unit main_unit = u.package->units["main"];
        REQUIRE(main_unit.type == U_TYPE::MODULE);

        REQUIRE(u.package->units.count("module") == 1);
        Unit module_unit = u.package->units["module"];
        REQUIRE(module_unit.type == U_TYPE::MODULE);

        REQUIRE(u.package->units.count("subpackage") == 1);
        Unit subpackage_unit = u.package->units["subpackage"];
        REQUIRE(subpackage_unit.type == U_TYPE::PACKAGE);

        Package* subpackage = subpackage_unit.package;
        REQUIRE(subpackage->units.count("moduleA") == 1);
        Unit moduleA_unit = subpackage->units["moduleA"];
        CHECK(moduleA_unit.type == U_TYPE::MODULE);

        REQUIRE(subpackage->units.count("moduleB") == 1);
        Unit moduleB_unit = subpackage->units["moduleB"];
        CHECK(moduleB_unit.type == U_TYPE::MODULE);

    }
    cleanup_dirs();
}

TEST_CASE("project_parse_test", "[compiler]") {
    setup_dirs();
    write_file(path_join(IN_DIR, "requirements.txt"), "core==1.0.0");
    Compiler c(IN_DIR, OUT_DIR, OUT_NAME, LIB_PATH, IS_LIB, VERSION);

    SECTION("parse one module ok") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");

        load_package(c.root_package, 1);
        bool parse_ok = parse_module(*c.root_package.units["main"].module);
        REQUIRE(parse_ok);
    }

    SECTION("parse one module error") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return1 0;}");

        load_package(c.root_package, 1);
        bool parse_ok = parse_module(*c.root_package.units["main"].module);
        REQUIRE(not parse_ok);
    }

    SECTION("parse with subpackage ok") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "module.xl"), "fun bar()->Integer{return 0;}");

        const std::string& SUBPACKAGE_PATH = path_join(IN_DIR, "subpackage");
        make_dir(SUBPACKAGE_PATH);
        write_file(path_join(SUBPACKAGE_PATH, "moduleA.xl"), "fun baz()->Integer{return 0;}");
        write_file(path_join(SUBPACKAGE_PATH, "moduleB.xl"), "fun foobar()->Integer{return 0;}");

        load_package(c.root_package, 1);

        bool parse_ok = parse_package(c.root_package);
        REQUIRE(parse_ok);
    }

    SECTION("parse with subpackage error") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "module.xl"), "fun bar()->Integer{return 0;}");

        const std::string& SUBPACKAGE_PATH = path_join(IN_DIR, "subpackage");
        make_dir(SUBPACKAGE_PATH);
        write_file(path_join(SUBPACKAGE_PATH, "moduleA.xl"), "fun baz()->Integer{return1 0;}");
        write_file(path_join(SUBPACKAGE_PATH, "moduleB.xl"), "fun foobar()->Integer{return 0;}");

        load_package(c.root_package, 1);

        bool parse_ok = parse_package(c.root_package);
        REQUIRE(not parse_ok);
    }
    cleanup_dirs();
}


TEST_CASE("project_preprocess_test", "[compiler]") {
    setup_dirs();
    write_file(path_join(IN_DIR, "requirements.txt"), "core==1.0.0");
    Compiler c(IN_DIR, OUT_DIR, OUT_NAME, LIB_PATH, IS_LIB, VERSION);

    SECTION("preprocess one module ok") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));

        REQUIRE(preprocess_package(c.root_package));
    }

    SECTION("parse one module error") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}fun foo()->Integer{}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));

        REQUIRE(not preprocess_package(c.root_package));
    }

    SECTION("preprocess with subpackage ok") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "module.xl"), "fun bar()->Integer{return 0;}");

        const std::string& SUBPACKAGE_PATH = path_join(IN_DIR, "subpackage");
        make_dir(SUBPACKAGE_PATH);
        write_file(path_join(SUBPACKAGE_PATH, "moduleA.xl"), "fun baz()->Integer{return 0;}");
        write_file(path_join(SUBPACKAGE_PATH, "moduleB.xl"), "fun foobar()->Integer{return 0;}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));

        REQUIRE(preprocess_package(c.root_package));
    }

    SECTION("preprocess with subpackage error") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "module.xl"), "fun bar()->Integer{return 0;}");

        const std::string& SUBPACKAGE_PATH = path_join(IN_DIR, "subpackage");
        make_dir(SUBPACKAGE_PATH);
        write_file(path_join(SUBPACKAGE_PATH, "moduleA.xl"), "fun baz()->Integer{return 0;}");
        write_file(path_join(SUBPACKAGE_PATH, "moduleB.xl"),
                   "fun foobar()->Integer{return 0;} fun foobar()->Integer{}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));

        REQUIRE(not preprocess_package(c.root_package));
    }
    cleanup_dirs();
}

TEST_CASE("import_test", "[compiler]") {
    setup_dirs();
    const std::string& REQUIREMENTS_PATH = path_join(IN_DIR, "requirements.txt");
    write_file(REQUIREMENTS_PATH, "libcore==1.0.0");
    Compiler c(IN_DIR, OUT_DIR, OUT_NAME, LIB_PATH, IS_LIB, VERSION);

    SECTION("import module ok") {
        write_file(path_join(IN_DIR, "main.xl"), "import test_program.moduleA; fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "moduleA.xl"), "fun bar()->Integer{return 0;}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));
        REQUIRE(preprocess_package(c.root_package));
        c.load_requirements(REQUIREMENTS_PATH);
        REQUIRE_NOTHROW(resolve_module_imports(*c.root_package.units["main"].module, c.top_package));
    }

    SECTION("import module error") {
        write_file(path_join(IN_DIR, "main.xl"), "import test_program.moduleB; fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "moduleA.xl"), "fun bar()->Integer{return 0;}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));
        REQUIRE(preprocess_package(c.root_package));
        c.load_requirements(REQUIREMENTS_PATH);
        REQUIRE_THROWS(resolve_module_imports(*c.root_package.units["main"].module, c.top_package));
    }

    SECTION("import function ok") {
        write_file(path_join(IN_DIR, "main.xl"), "import test_program.moduleA.bar; fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "moduleA.xl"), "fun bar()->Integer{return 0;}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));
        REQUIRE(preprocess_package(c.root_package));
        c.load_requirements(REQUIREMENTS_PATH);
        REQUIRE_NOTHROW(resolve_module_imports(*c.root_package.units["main"].module, c.top_package));
    }

    SECTION("import function not found") {
        write_file(path_join(IN_DIR, "main.xl"), "import test_program.moduleA.foobar; fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "moduleA.xl"), "fun bar()->Integer{return 0;}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));
        REQUIRE(preprocess_package(c.root_package));
        c.load_requirements(REQUIREMENTS_PATH);
        REQUIRE_THROWS(resolve_module_imports(*c.root_package.units["main"].module, c.top_package));
    }

    SECTION("import class ok") {
        write_file(path_join(IN_DIR, "main.xl"), "import test_program.moduleA.Bar; fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "moduleA.xl"), "class Bar{x: Integer;}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));
        REQUIRE(preprocess_package(c.root_package));
        c.load_requirements(REQUIREMENTS_PATH);
        REQUIRE_NOTHROW(resolve_module_imports(*c.root_package.units["main"].module, c.top_package));
    }

    SECTION("import class error") {
        write_file(path_join(IN_DIR, "main.xl"), "import test_program.moduleA.FooBar; fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "moduleA.xl"), "class Bar{x: Integer;}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));
        REQUIRE(preprocess_package(c.root_package));
        c.load_requirements(REQUIREMENTS_PATH);
        REQUIRE_THROWS(resolve_module_imports(*c.root_package.units["main"].module, c.top_package));
    }

    SECTION("import package error") {
        write_file(path_join(IN_DIR, "main.xl"), "import foo_package; fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "moduleA.xl"), "class Bar{x: Integer;}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));
        REQUIRE(preprocess_package(c.root_package));
        c.load_requirements(REQUIREMENTS_PATH);
        REQUIRE_THROWS(resolve_module_imports(*c.root_package.units["main"].module, c.top_package));
    }

    SECTION("import package ok") {
        write_file(path_join(IN_DIR, "main.xl"), "import test_program; fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "moduleA.xl"), "class Bar{x: Integer;}");

        load_package(c.root_package, 1);
        REQUIRE(parse_package(c.root_package));
        REQUIRE(preprocess_package(c.root_package));
        c.load_requirements(REQUIREMENTS_PATH);
        REQUIRE_NOTHROW(resolve_module_imports(*c.root_package.units["main"].module, c.top_package));
    }
    cleanup_dirs();
}
