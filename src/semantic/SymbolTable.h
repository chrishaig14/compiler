//
// Created by chris on 1/7/20.
//

#ifndef UNTITLED1_SYMBOLTABLE_H
#define UNTITLED1_SYMBOLTABLE_H

#include <string>
#include <vector>
#include <map>
#include "../nodes/nodes.h"
#include "../utils.h"
#include "SymbolInfo.h"


class SymbolTable {
    std::map<std::string, SymbolInfo*> table;
    std::string name;
    SymbolTable* parent;
public:
    SymbolTable(std::string name, SymbolTable* parent) {
        this->name = name;
        this->parent = parent;
    }


    bool has(std::string name) {
        if (this->table.count(name) == 1) {
            return true;
        } else {
            if (this->parent != nullptr) {
                return this->parent->has(name);
            }
            return false;
        }
    }

    SymbolInfo* get(std::string name) {
        if (this->table.count(name) == 1) {
            return this->table[name];
        } else {
            if (this->parent != nullptr) {
                return this->parent->get(name);
            }
        }
        throw std::runtime_error("Symbol " + name + " not found in scope");
    }

    bool declared(std::string name) {
        return this->table.count(name) == 1;
    }

    void set(std::string name, SymbolInfo* info) {
        this->table[name] = info;
    }


};

#endif //UNTITLED1_SYMBOLTABLE_H
