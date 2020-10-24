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
    std::map<std::string, TypeNode*> table;
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

    TypeNode* get(std::string name) {
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

    void set(std::string name, TypeNode* info) {
        ObjectTypeNode* otn = dynamic_cast<ObjectTypeNode*>(info);
        if (otn != nullptr) {
            if (otn->identifier == "Option") {
                this->not_null[name] = false;
            }
        }
        this->table[name] = info;
    }

    void set_not_none(std::string name, bool may_be_none) {
        this->not_null[name] = may_be_none;
    }

    bool get_not_none(std::string name) {
        if (this->not_null.count(name) == 1) {
            return this->not_null[name];
        } else {
            if (this->parent != nullptr) {
                return this->parent->get_not_none(name);
            }
        }
        throw std::runtime_error("Symbol " + name + " not found in scope");
    }


    std::string name;
    SymbolTable* parent;
    std::map<std::string, bool> not_null;
};

#endif //UNTITLED1_SYMBOLTABLE_H
