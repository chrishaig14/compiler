#include <iostream>
#include <fstream>
#include <sstream>
#include "src/scanner/Scanner.h"
#include "src/parser/Parser.h"
#include "src/formatter/Formatter.h"
#include "src/semantic/GlobalProcessor.h"
#include "src/semantic/Checker.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: expected a file name to compile" << std::endl;
        exit(1);
    }
    std::string filename = argv[1];
    std::ifstream file(filename);
    std::stringstream sstream;
    sstream << file.rdbuf();
    std::string text = sstream.str();
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* ast = parser.parse_program();
    GlobalProcessor gp;
    gp.visit(*ast);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*ast);
    return 0;
}