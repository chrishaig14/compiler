//
// Created by chris on 21/7/20.
//

#include "Environment.h"

Environment::Environment(std::string name, Environment* parent) {
    this->parent = parent;
    this->name = name;
}

void Environment::set(std::string name, Object* value) {
    if (this->table.count(name) == 0) {
        if (this->parent != nullptr) {
            this->parent->set(name, value);
        } else {
            throw std::runtime_error("Error name " + name + " not found in current environment");
        }
    }
    this->table[name] = value;
}

Object* Environment::get(std::string name) {
    if (this->table.count(name) == 0) {
        if (this->parent != nullptr) {
            return this->parent->get(name);
        } else {
            throw std::runtime_error("Error name " + name + " not found in current scope");
        }
    }
    if (this->table[name] == nullptr) {
        throw std::runtime_error("Error name " + name + " has nullptr");
    }
    return this->table[name];
}

void Environment::declare(std::string name) {
    if (this->table.count(name) == 1) {
        throw std::runtime_error("Error name " + name + " already declared in current environment");
    }
    this->table[name] = nullptr;
}

Environment* Environment::enter(std::string name) {
    return new Environment(name, this);
}

Environment* Environment::leave(std::string name) {
    if (this->name == name) return this->parent;
    return this->parent->leave(name);
}

bool Environment::is_declared(std::string name) {
    return this->table.count(name) == 1;
}
