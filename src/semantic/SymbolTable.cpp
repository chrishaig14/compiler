//
// Created by chris on 1/7/20.
//

#include "SymbolTable.h"
#include "../units/FunctionValue.h"
#include "../units/ObjectValue.h"

SymbolTable::SymbolTable(const std::string& name, SymbolTable* parent) {
    this->name = name;
    this->parent = parent;
    this->ret = nullptr;
    this->is_function = false;
    this->is_loop = false;
}

Entity SymbolTable::get(const std::string& name) {
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
        return it->second;
    }
    // this->
    if (this->parent != nullptr) {
        return this->parent->get(name);
    }
    // throw std::runtime_error("Error path: " + name + " not found!");
    return Entity(E_TYPE::NOT_FOUND);
}

bool SymbolTable::declared(const std::string& name) {
    return this->table.find(name) != this->table.end();
}

void SymbolTable::set(const std::string& name, Entity info) {
    if (name == "__return__") {
        this->ret = new Entity{};
        *this->ret = info;
        return;
    }
    this->table[name] = info;
}

std::vector<std::pair<std::string, TypeNode*>> SymbolTable::get_all_in_loop() {
    if (this->is_loop) {
        std::vector<std::pair<std::string, TypeNode*>> r;

        for (auto v: this->table) {
            TypeNode* t;
            // if (v.second->type == E_TYPE::FUNCTION_VALUE) {
            //     t = ((FunctionValue*) v.second)->ft;
            // }
            // if (v.second->type == E_TYPE::OBJECT_VALUE) {
            //     t = ((ObjectValue*) v.second)->ot;
            // }
            r.push_back(std::make_pair(v.first, t));
        }
        return r;
    } else {
        std::vector<std::pair<std::string, TypeNode*>> r;

        auto p = this->parent->get_all_in_loop();
        r.insert(r.end(), p.begin(), p.end());
        for (auto v: this->table) {
            TypeNode* t;
            // if (v.second->type == E_TYPE::FUNCTION_VALUE) {
            //     t = ((FunctionValue*) v.second)->ft;
            // }
            // if (v.second->type == E_TYPE::OBJECT_VALUE) {
            //     t = ((ObjectValue*) v.second)->ot;
            // }
            r.push_back(std::make_pair(v.first, t));
        }
        return r;
    }
}

std::vector<std::pair<std::string, TypeNode*>> SymbolTable::get_all() {
    if (this->is_function) {
        std::vector<std::pair<std::string, TypeNode*>> r;

        for (auto v: this->table) {
            Entity e = v.second;
            TypeNode* t;
            if (e.type == E_TYPE::VALUE) {
                TypeNode& type = *e.value->type;
                t = type.clone();
                if (e.type == E_TYPE::VALUE) {
                    r.push_back(std::make_pair(v.first, t));
                }
            }

        }
        return r;
    } else {
        std::vector<std::pair<std::string, TypeNode*>> r;

        auto p = this->parent->get_all();
        r.insert(r.end(), p.begin(), p.end());
        // for (auto v: this->table) {
        //     TypeNode* t;
        //     if (v.second->type == E_TYPE::FUNCTION_VALUE) {
        //         t = ((FunctionValue*) v.second)->ft;
        //     }
        //     if (v.second->type == E_TYPE::OBJECT_VALUE) {
        //         t = ((ObjectValue*) v.second)->ot;
        //     }
        //     r.push_back(std::make_pair(v.first, t));
        // }
        return r;
    }
}
