#include <iostream>
#include <fstream>
#include <sstream>
#include "src/scanner/Scanner.h"
#include "src/parser/Parser.h"
#include "src/formatter/Formatter.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: expected a file name to format and an output file" << std::endl;
        exit(1);
    }
    std::string filename = argv[1];
    std::ifstream file(filename);
    std::stringstream sstream;
    sstream << file.rdbuf();
    std::string text = sstream.str();
//    std::cerr <<  "The input file is: " << std::endl << text << std::endl;
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* ast = parser.parse_program();
    Formatter formatter;
    ast->accept(formatter);
    std::cerr <<  formatter.output;
    std::ofstream output(argv[2]);
    output << formatter.output;
    return 0;
}