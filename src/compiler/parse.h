//
// Created by chris on 25/5/21.
//

#ifndef XLANG_PARSE_H
#define XLANG_PARSE_H

#include "../nodes/BlockNode.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"

void parse_single_module(Module& module);

void parse_all_modules(Package& package);

#endif //XLANG_PARSE_H
