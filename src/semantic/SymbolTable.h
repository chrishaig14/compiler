//
// Created by chris on 1/7/20.
//

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <vector>
#include <map>
#include "../nodes/TypeNode.h"
#include "../utils.h"
#include "SymbolInfo.h"


class SymbolTable {
    std::map<std::string, TypeNode*> table;
public:
    SymbolTable(std::string name, SymbolTable* parent);


    bool has(std::string name);

    const TypeNode& get(std::string name);

    bool declared(std::string name);

    void set(std::string name, const TypeNode& info);

    void set_not_none(std::string name, bool may_be_none);

    bool get_not_none(std::string name);


    std::string name;
    SymbolTable* parent;
    std::map<std::string, bool> not_null;
};

#endif //SYMBOLTABLE_H
