//
// Created by chris on 1/7/20.
//

#ifndef UNTITLED1_SYMBOLTABLE_H
#define UNTITLED1_SYMBOLTABLE_H

#include <string>
#include <vector>
#include <map>
#include "AstNode.h"

class ObjectInfo;

class FunctionInfo;

class ObjectInfo {
public:
    std::string parent;
    std::vector<ObjectInfo*> type_parameters;

    ObjectInfo(TypeNode* n);
};

class FunctionInfo {
public:
    std::vector<ObjectInfo*> parameter_types;
    ObjectInfo* return_type;

    FunctionInfo(std::vector<TypeNode*> parameter_types, TypeNode* return_type);
};

class ClassInfo {
public:
    std::map<std::string, ObjectInfo*> fields;
    std::map<std::string, FunctionInfo*> methods;

    ClassInfo(std::map<std::string, ObjectInfo*> fields,
              std::map<std::string, FunctionInfo*> methods) {
        this->fields = fields;
        this->methods = methods;
    }

    ClassInfo() {}

};

enum class SINFO {
    CLASS, FUNCTION, SIMPLE
};

class SymbolInfo {
public:
    union {
        FunctionInfo* function_info;
        ObjectInfo* object_info;
    };
    SINFO type;

    SymbolInfo(FunctionInfo* function_info) : function_info(function_info) {
        this->type = SINFO::FUNCTION;
    }

    SymbolInfo(ObjectInfo* object_info) : object_info(object_info) {
        this->type = SINFO::SIMPLE;
    }

    bool is_function() { return this->type == SINFO::FUNCTION; }

    bool is_object() { return this->type == SINFO::SIMPLE; }

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

    std::string name;
    SymbolTable* parent;
};

#endif //UNTITLED1_SYMBOLTABLE_H
