//
// Created by chris on 1/7/20.
//

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include "../ast/Type.h"
#include "../utils.h"
#include "SemanticInfo.h"


class SymbolTable {
    Entity* ret;

public:
    SymbolTable(const std::string& name, SymbolTable* parent);

    std::vector<std::pair<std::string, ast::Type*>> get_all();

    Entity& get(const std::string& name);

    bool declared(const std::string& name);

    void set(const std::string& name, Entity* info);

    ~SymbolTable() {
        // for (auto e: this->table) {
        //     delete e.second;
        // }
        // if (this->ret != nullptr) {
        //     delete ret;
        // }

    }

    std::string s_name;
    SymbolTable* parent;
    std::unordered_map<std::string, bool> not_null;
    MapStringEntity table;
    bool is_function;

    bool is_loop;
    std::vector<std::pair<std::string, ast::Type*>> get_all_in_loop();
};

#endif //SYMBOLTABLE_H
