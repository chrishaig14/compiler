//
// Created by chris on 4/12/20.
//

#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <semantic/GlobalProcessor.h>
#include <semantic/Checker.h>
#include "utils_test.h"


BlockNode* get_ast(std::string text) {
    Scanner scanner(text);
    std::vector <Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* tree = parser.parse_program();
    return tree;
}

void compile(std::string text) {
    BlockNode* tree = get_ast(text);
    std::vector <std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit_block(*tree);
    Checker checker(gp.globals, gp.global_classes, gp.global_functions);
    checker.visit_block(*tree);
}

