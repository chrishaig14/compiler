#include "catch.hpp"
#include "../src/scanner/Scanner.h"
#include "../src/parser/Parser.h"
#include "../src/nodes/UnaryOpNode.h"
#include "../src/semantic/GlobalProcessor.h"

const ObjectType NO_TYPE(".None");

TEST_CASE("global", "[parser]") {
    Scanner scanner;
    std::string code = "fun main()->Integer{return 0;}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);
    parser.top_package_name = "main";
    Module module(Path("main.foo"), "foo.xl", ".", false, "main.h");
    module.ast = parser.parse_program();
    GlobalProcessor gp(&module);
    gp.visit_root(*module.ast);
    REQUIRE(module.flirpins.size() == 1);
    REQUIRE(module.flirpins.count("main") == 1);
}