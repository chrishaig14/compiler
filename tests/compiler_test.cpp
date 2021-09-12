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
        c.load_project();
        REQUIRE(c.top_package.units.count(OUT_NAME) == 1);
        Unit u = c.top_package.units[OUT_NAME];
        REQUIRE(u.type == U_TYPE::PACKAGE);

        REQUIRE(u.package->units.size() == 1);

        REQUIRE(u.package->units.count("main") == 1);
        Unit m = u.package->units["main"];
        REQUIRE(m.type == U_TYPE::MODULE);
    }SECTION("multiple modules") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "module.xl"), "fun bar()->Integer{return 0;}");
        c.load_project();
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

    SECTION("one subpackage no modules") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");
        write_file(path_join(IN_DIR, "module.xl"), "fun bar()->Integer{return 0;}");


        const std::string& SUBPACKAGE_PATH = path_join(IN_DIR, "subpackage");
        make_dir(SUBPACKAGE_PATH);

        write_file(path_join(SUBPACKAGE_PATH, "moduleA.xl"), "fun baz()->Integer{return 0;}");
        write_file(path_join(SUBPACKAGE_PATH, "moduleB.xl"), "fun foobar()->Integer{return 0;}");

        c.load_project();
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
