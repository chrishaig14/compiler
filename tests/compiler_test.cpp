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
    int r = mkdir(IN_DIR.c_str(), 0700);
    REQUIRE(r == 0);
}

void cleanup_dirs() {
    std::string cmd = "rm -rf " + IN_DIR;
    system(cmd.c_str());
    cmd = "rm -rf " + OUT_DIR;
    system(cmd.c_str());
}

TEST_CASE("modules_test", "[compiler]") {
    setup_dirs();
    write_file(path_join(IN_DIR, "requirements.txt"), "core==1.0.0");
    write_file(path_join(IN_DIR, "module.xl"), "fun bar()->Integer{return 7;}");
    Compiler c(IN_DIR, OUT_DIR, OUT_NAME, LIB_PATH, IS_LIB, VERSION);
    SECTION("single module ok") {
        write_file(path_join(IN_DIR, "main.xl"), "fun foo()->Integer{return 0;}");
    }SECTION("import error when imported package not found") {
        write_file(path_join(IN_DIR, "main.xl"), "import non_existent_package;fun foo()->Integer{return 0;}");
    }SECTION("import error when imported module not found") {
        write_file(path_join(IN_DIR, "main.xl"),
                   "import test_program.non_existent_module_or_package;fun foo()->Integer{return 0;}");
    }SECTION("import error when imported function not found") {
        write_file(path_join(IN_DIR, "main.xl"), "import test_program.module.baz;fun foo()->Integer{return 0;}");
    }SECTION("import error when imported class not found") {
        write_file(path_join(IN_DIR, "main.xl"), "import test_program.module.Baz;fun foo()->Integer{return 0;}");
    }
    c.pre();
    c.main();
    cleanup_dirs();
}
