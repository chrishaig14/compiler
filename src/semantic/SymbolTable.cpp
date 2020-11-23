//
// Created by chris on 1/7/20.
//

#include "SymbolTable.h"

SymbolTable::SymbolTable(const std::string& name, SymbolTable* parent) {
    this->name = name;
    this->parent = parent;
}

bool SymbolTable::has(const std::string& name) {
    if (this->table.find(name) != this->table.end()) {
        return true;
    } else {
        if (this->parent != nullptr) {
            return this->parent->has(name);
        }
        return false;
    }
}

const TypeNode& SymbolTable::get(const std::string& name) {
    auto it = this->table.find(name);
    if (it != this->table.end()) {
        return *it->second;
    } else {
        if (this->parent != nullptr) {
            return this->parent->get(name);
        }
    }
    throw std::runtime_error("Symbol " + name + " not found in scope");
}

bool SymbolTable::declared(const std::string& name) {
    return this->table.find(name) != this->table.end();
}

void SymbolTable::set(const std::string& name, const TypeNode& info) {
    if (info.kind == Kind::OBJECT) {
        if (info.object().identifier == "Option") {
            this->not_null[name] = false;
        }
    }
    this->table[name] = info.clone();
}

void SymbolTable::set_not_none(const std::string& name, bool may_be_none) {
    this->not_null[name] = may_be_none;
}

bool SymbolTable::get_not_none(const std::string& name) {
    if (this->not_null.find(name) != this->not_null.end()) {
        return this->not_null[name];
    } else {
        if (this->parent != nullptr) {
            return this->parent->get_not_none(name);
        }
    }
    throw std::runtime_error("Symbol " + name + " not found in scope");
}
