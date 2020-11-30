//
// Created by chris on 21/7/20.
//

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <unordered_set>
#include "Object.h"

class Environment {
    std::unordered_map<std::string, Object*> table;

    Environment* parent;
    std::string id;
    std::unordered_set<Object*> reachable;
public:
    Environment(const std::string& id, Environment* parent);

    void set(const std::string& name, Object* value);

    bool is_declared(const std::string& name);

    Object* get(const std::string& name);

    void declare(const std::string& name);

    Environment* enter(const std::string& name);

    const std::unordered_set<Object*>& get_directly_reachable() {
        reachable = {};
        for (auto e: this->table) {
            reachable.insert(e.second);
        }
        if (this->parent != nullptr) {
            const std::unordered_set<Object*>& parent_reachable = parent->get_directly_reachable();
            reachable.insert(
                    parent_reachable.begin(),
                    parent_reachable.end());
        }
        return reachable;
    }


    Environment* leave(const std::string& name);
};

#endif //ENVIRONMENT_H
