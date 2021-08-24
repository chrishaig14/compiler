//
// Created by chris on 18/3/21.
//

#include "util.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"

UTypeNode parse_type(const std::string& s) {
    Scanner scanner;
    scanner.load_text(s);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("", scanner.code_lines, tokens);
    return parser.parse_type_node();
}

FunctionType* parse_function_type(const std::string& s) {
    Scanner scanner;
    scanner.load_text(s);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("", scanner.code_lines, tokens);
    return parser.parse_function_type().release();
}