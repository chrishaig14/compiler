//
// Created by chris on 6/10/20.
//

#ifndef UNTITLED1_TYPECLASSINFO_H
#define UNTITLED1_TYPECLASSINFO_H


#include <string>
#include "SymbolInfo.h"

class TypeClassInfo {

public:
    std::string type_name;
    std::map<std::string, FunctionTypeNode*> functions;

    bool has_function(std::string function_name) {
        return this->functions.count(function_name) == 1;
    }
};


#endif //UNTITLED1_TYPECLASSINFO_H
