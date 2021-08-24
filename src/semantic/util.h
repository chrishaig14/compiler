//
// Created by chris on 18/3/21.
//

#ifndef XLANG_UTIL_H
#define XLANG_UTIL_H

#include "../nodes/TypeNode.h"

UTypeNode parse_type(const std::string& s);

FunctionType* parse_function_type(const std::string& s);
#endif //XLANG_UTIL_H
