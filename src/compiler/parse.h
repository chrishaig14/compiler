//
// Created by chris on 25/5/21.
//

#ifndef XLANG_PARSE_H
#define XLANG_PARSE_H

#include "../nodes/BlockNode.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"

BlockNode* full_parse(const std::string& __file__, CodeLines* code_lines);

void parse_all_modules(Package* package);

#endif //XLANG_PARSE_H
