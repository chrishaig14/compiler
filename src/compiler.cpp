#include <iostream>
#include <fstream>
#include <sstream>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "semantic/GlobalProcessor.h"
#include "semantic/Checker.h"

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
    GlobalProcessor gp(nullptr, nullptr, std::string(), std::map<std::string, std::string>());
    gp.visit(*ast);
    Checker checker(gp.globals, gp.global_classes);
    checker.visit(*ast);
    return 0;
}