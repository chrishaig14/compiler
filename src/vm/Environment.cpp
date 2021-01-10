//
// Created by chris on 21/7/20.
//

#include "Environment.h"

Environment::Environment(const std::string& id, Environment* parent) {
    this->parent = parent;
    this->id = id;
}

void Environment::set(const std::string& name, Object* value) {
//    std::unordered_map<std::string, Object*>::iterator it = this->table.find(name);
//    if (it == this->table.end()) {
//        // not already in table
//        if (this->parent != nullptr) {
//            this->parent->set(name, value);
//        } else {
//            throw std::runtime_error("Error name " + name + " not found in current environment");
//        }
//    } else {
//        this->variables[this->name_to_index[name]] = value;
//        it->second = value;
//    }
}

void Environment::set_with_location(const std::string& name, Object* value, VariableLocation location) {
    assert(location.depth != -1);
    if (location.depth == -2) {
        this->table[name] = value;
        return;
    }
    if (location.depth == 0) {
        if (location.index > (int) this->variables.size()) {
            throw std::runtime_error("Error: setting " + name + " with location out of range");
        }
        if (location.index == -1) {
            // declaring new var
            this->variables.push_back(value);
        } else {
            this->variables[location.index] = value;
        }
        this->table[name] = value;
    } else {
        if (this->parent != nullptr) {
            location.depth--;
            this->parent->set_with_location(name, value, location);
        } else {
            throw std::runtime_error("Error: setting " + name + " with location in ancestor but parent is NULL");
        }
    }
//    } else {
//        std::unordered_map<std::string, Object*>::iterator it = this->table.find(name);
//        if (it == this->table.end()) {
//            // not already in table
//            if (this->parent != nullptr) {
//                this->parent->set(name, value);
//            } else {
//                throw std::runtime_error("Error name " + name + " not found in current environment");
//            }
//        } else {
//            this->variables[this->name_to_index[name]] = value;
//
//            it->second = value;
//        }
//    }
}

//
//Object* Environment::get(const std::string& name) {
//    auto it = this->table.find(name);
//    if (it == this->table.end()) {
//        if (this->parent != nullptr) {
//            return this->parent->get(name);
//        } else {
//            throw std::runtime_error("Error name " + name + " not found in current scope");
//        }
//    }
//    if (it->second == nullptr) {
//        throw std::runtime_error("Error name " + name + " has nullptr");
//    }
//    return it->second;
//}

void Environment::declare(const std::string& name) {
    if (this->table.find(name) != this->table.end()) {
        throw std::runtime_error("Error name " + name + " already declared in current environment");
    }
    this->name_to_index[name] = this->variables.size();
    this->index_to_name[this->variables.size()] = name;
    this->variables.push_back(nullptr);
    this->table[name] = nullptr;
}

Environment* Environment::enter(const std::string& name) {
    Environment* e = new Environment(name, this);
    e->global = this->global;
    return e;
}

Environment* Environment::leave(const std::string& name) {
    if (this->id == name) {
        return this->parent;
    }
    return this->parent->leave(name);
}

bool Environment::is_declared(const std::string& name) {
    return this->table.find(name) != this->table.end();
}

Object* Environment::get_with_location(const std::string& name, VariableLocation location) {
//    return this->get(name);
    assert(location.depth != -1);
    if (location.depth == -2) {

        // It's a global
        if (this->parent != nullptr) {
            return this->global->get_with_location(name, location);
        } else {
            // IM the global
            return this->table.find(name)->second;
        }
    }
    if (location.depth == 0) {
        Object* value = this->variables[location.index];
        if (value == nullptr) {
            throw std::runtime_error(
                    "RETURNING NULL FOR " + name + " (D: " + std::to_string(location.depth) + " I: " +
                    std::to_string(location.index) + ")"
            );
        }
        return value;
    } else {
        if (this->parent != nullptr) {
            location.depth--;
            return this->parent->get_with_location(name, location);
        } else {
            throw std::runtime_error("ERROR GETTING WITH LOCATION BUT PARENT IS NULL");
        }
    }
}

