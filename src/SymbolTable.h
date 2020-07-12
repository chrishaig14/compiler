//
// Created by chris on 1/7/20.
//

#ifndef UNTITLED1_SYMBOLTABLE_H
#define UNTITLED1_SYMBOLTABLE_H

#include <string>
#include <vector>
#include <map>
#include "AstNode.h"

class SimpleInfo;

class FunctionInfo;

class SimpleInfo {
public:
    std::string parent;
    std::vector<SimpleInfo*> type_parameters;

    SimpleInfo(TypeNode* n);
};

class FunctionInfo {
public:
    std::vector<SimpleInfo*> parameter_types;
    SimpleInfo* return_type;

    FunctionInfo(std::vector<TypeNode*> parameter_types, TypeNode* return_type);
};

class ClassInfo {
public:
    std::map<std::string, SimpleInfo*> fields;
    std::map<std::string, FunctionInfo*> methods;
};

enum class SINFO {
    CLASS, FUNCTION, SIMPLE
};

class SymbolInfo {
public:
    union {
        ClassInfo* class_info;
        FunctionInfo* function_info;
        SimpleInfo* simple_info;
    };
    SINFO type;
};

class SymbolTable {
    std::map<std::string, SymbolInfo*> table;
public:
    SymbolTable(std::string name, SymbolTable* parent) {
        this->name = name;
        this->parent = parent;
    }

    bool has(std::string name) {
        if (this->table.count(name) == 1) {
            return true;
        } else {
            if (this->parent != NULL) {
                return this->parent->has(name);
            }
            return false;
        }
    }

    SymbolInfo* get(std::string name) {
        if (this->table.count(name) == 1) {
            return this->table[name];
        } else {
            if (this->parent != NULL) {
                return this->parent->get(name);
            }
        }
        throw "Symbol" + name + "not found in scope";
    }

    bool declared(std::string name) {
        return this->table.count(name) == 1;
    }

    void set(std::string name, SymbolInfo* info) {
        this->table[name] = info;
    }

    std::string name;
    SymbolTable* parent;
};

#endif //UNTITLED1_SYMBOLTABLE_H
