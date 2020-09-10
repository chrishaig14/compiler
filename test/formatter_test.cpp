#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <formatter/Formatter.h>
#include <nodes/BlockNode.h>

TEST(formatter_test, test_1) {
    std::string filename = "../../test_file.pl";
    std::ifstream file(filename);
    if (!file.is_open()) {
        FAIL() << "Couldn't open file";
    }
    std::stringstream sstream;
    sstream << file.rdbuf();
    std::string text = sstream.str();
    std::cerr << "The input file is: \n" + text + "\n";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* ast = parser.parse_program();
    Formatter formatter;
    ast->accept(formatter);
    std::cerr << formatter.output;
}