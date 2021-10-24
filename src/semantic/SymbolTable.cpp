//
// Created by chris on 1/7/20.
//

#include "SymbolTable.h"
#include <units/entities/EntityNotFound.h>
#include <units/entities/EntityValue.h>

SymbolTable::SymbolTable(SymbolTable* parent) {
    this->parent = parent;
    this->ret = nullptr;
    this->is_function = false;
    this->is_loop = false;
}

Entity& SymbolTable::get(const std::string& name) {
    if (name == "__return__") {
        if (this->ret == nullptr) {
            assert(this->parent!=nullptr);
            return this->parent->get(name);
        }
        return *this->ret;
    }
    auto it = this->table.find(name);
    if (it != this->table.end()) {
        return *it->second;
    }
    if (this->parent != nullptr) {
        return this->parent->get(name);
    }
    return *new EntityNotFound();
}

bool SymbolTable::declared(const std::string& name) {
    return this->table.find(name) != this->table.end();
}

void SymbolTable::set(const std::string& name, const Entity& info) {
    if (name == "__return__") {
        this->ret = std::unique_ptr<Entity>(info.clone());
        return;
    }
    this->table[name] = std::unique_ptr<Entity>(info.clone());
}

std::vector<std::pair<std::string, ast::Type*>> SymbolTable::get_all_in_loop() {
    if (this->is_loop) {
        std::vector<std::pair<std::string, ast::Type*>> r;

        for (auto& v: this->table) {
            ast::Type* t;
            r.push_back(std::make_pair(v.first, t));
        }
        return r;
    } else {
        std::vector<std::pair<std::string, ast::Type*>> r;

        auto p = this->parent->get_all_in_loop();
        r.insert(r.end(), p.begin(), p.end());
        for (auto& v: this->table) {
            ast::Type* t;
            r.push_back(std::make_pair(v.first, t));
        }
        return r;
    }
}

std::vector<std::pair<std::string, sem::UType>> SymbolTable::get_all() {
    if (this->is_function) {
        std::vector<std::pair<std::string, sem::UType>> r;

        for (auto& v: this->table) {
            Entity& e = *v.second;
            if (e.is_value()) {
                auto& ev = e.get_value();
                r.emplace_back(v.first, sem::UType(ev.type.clone()));
            }

        }
        return r;
    } else {
        std::vector<std::pair<std::string, sem::UType>> r;
        return r;
    }
}
