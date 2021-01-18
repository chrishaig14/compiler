//
// Created by chris on 6/10/20.
//

#ifndef TYPECLASSINFO_H
#define TYPECLASSINFO_H


#include <string>
#include "SemanticInfo.h"

class TypeClassInfo {

public:
    std::string type_name;
    std::unordered_map<std::string, FunctionType*> functions;

    bool has_function(std::string function_name);
};


#endif //TYPECLASSINFO_H
