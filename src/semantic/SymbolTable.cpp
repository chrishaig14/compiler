//
// Created by chris on 1/7/20.
//

#include "SymbolTable.h"

SymbolTable::SymbolTable(const std::string& name, SymbolTable* parent) {
    this->name = name;
    this->parent = parent;
    this->ret = nullptr;
    this->is_function = false;
    this->is_loop = false;
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

VariableLocation SymbolTable::find(const std::string& name) {
    auto it = this->indices.find(name);
    if (it != this->indices.end()) {
        int index = it->second;
        return VariableLocation(0, index);
    } else {
        if (this->parent != nullptr) {
            VariableLocation location = this->parent->find(name);
            location.depth++;
            return location;
        }
        return VariableLocation(-1, -1);
    }
}

const TypeNode& SymbolTable::get(const std::string& name) {
    if (name == "__return__") {
        if (this->ret == nullptr) {
            if (this->parent != nullptr) {
                return this->parent->get(name);
            } else {
                throw std::runtime_error("ERROR: no parent but want __return__");
            }
        }
        return *this->ret;
    }
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
    if (name == "__return__") {
        this->ret = info.clone();
        return;
    }
    if (info.kind == Kind::OBJECT) {
        if (info.object().id == "Option") {
            this->not_null[name] = false;
        }
    }
    auto it = this->table.find(name);
    if (it == this->table.end()) {
        // it's new
        this->indices.insert({name, this->table.size()});
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

std::vector<std::pair<std::string, TypeNode*>> SymbolTable::get_all_in_loop() {
    if (this->is_loop) {
        std::vector<std::pair<std::string, TypeNode*>> r;

        for (auto v: this->table) {
            r.push_back(std::make_pair(v.first,v.second->clone()));
        }
        return r;
    } else {
        std::vector<std::pair<std::string, TypeNode*>> r;

        auto p = this->parent->get_all_in_loop();
        r.insert(r.end(), p.begin(), p.end());
        for (auto v: this->table) {
            r.push_back(std::make_pair(v.first,v.second->clone()));
        }
        return r;
    }
}

std::vector<std::pair<std::string, TypeNode*>> SymbolTable::get_all() {
    if (this->is_function) {
        std::vector<std::pair<std::string, TypeNode*>> r;

        for (auto v: this->table) {
            r.push_back(std::make_pair(v.first,v.second->clone()));
        }
        return r;
    } else {
        std::vector<std::pair<std::string, TypeNode*>> r;

        auto p = this->parent->get_all();
        r.insert(r.end(), p.begin(), p.end());
        for (auto v: this->table) {
            r.push_back(std::make_pair(v.first,v.second->clone()));
        }
        return r;
    }
}
