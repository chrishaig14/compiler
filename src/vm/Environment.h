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
    std::vector<Object*> variables;
    std::unordered_map<std::string, int> name_to_index;
    std::unordered_map<int, std::string> index_to_name;

    Environment* parent;
    std::string id;
    std::vector<Object*> reachable;
public:
    Environment(const std::string& id, Environment* parent);

    void set(const std::string& name, Object* value);
    void set_with_location(const std::string& name, Object* value, VariableLocation location);


    bool is_declared(const std::string& name);

    Object* get(const std::string& name);
    Object* get_with_location(const std::string& name, VariableLocation location);

    void declare(const std::string& name);

    Environment* enter(const std::string& name);

    const std::vector<Object*>& get_directly_reachable() {
        reachable = {};
        for (auto& e: this->table) {
            if (!FLAG_IS_SET(e.second->flags, REACHABLE)) {
                SET_FLAG(e.second->flags, REACHABLE);
                reachable.push_back(e.second);
            }
        }
        if (this->parent != nullptr) {
            const std::vector<Object*>& parent_reachable = parent->get_directly_reachable();
            reachable.insert(reachable.end(), parent_reachable.begin(), parent_reachable.end());
        }
        return reachable;
    }


    Environment* leave(const std::string& name);
};

#endif //ENVIRONMENT_H
