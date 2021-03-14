//
// Created by chris on 1/7/20.
//

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include "../nodes/TypeNode.h"
#include "../utils.h"
#include "SemanticInfo.h"


class SymbolTable {
    std::unordered_map<std::string, int> indices;
    TypeNode* ret;

public:
    SymbolTable(const std::string& name, SymbolTable* parent);

    VariableLocation find(const std::string& name);

    bool has(const std::string& name);

    std::vector<std::pair<std::string, TypeNode*>> get_all();

    const TypeNode& get(const std::string& name);

    bool declared(const std::string& name);

    void set(const std::string& name, const TypeNode& info);

    void set_not_none(const std::string& name, bool may_be_none);

    bool get_not_none(const std::string& name);

    ~SymbolTable() {
        for (auto e: this->table) {
            delete e.second;
        }
    }

    std::string name;
    SymbolTable* parent;
    std::unordered_map<std::string, bool> not_null;
    MapStringType table;
    bool is_function;

    bool is_loop;
    std::vector<std::pair<std::string, TypeNode*>> get_all_in_loop();
};

#endif //SYMBOLTABLE_H
