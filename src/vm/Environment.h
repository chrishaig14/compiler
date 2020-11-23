//
// Created by chris on 21/7/20.
//

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include "Object.h"

class Environment {
    std::unordered_map<std::string, Object*> table;

    Environment* parent;
    std::string id;
public:
    Environment(const std::string& id, Environment* parent);

    void set(const std::string& name, Object* value);

    bool is_declared(const std::string& name);

    Object* get(const std::string& name);

    void declare(const std::string& name);

    Environment* enter(const std::string& name);

    Environment* leave(const std::string& name);
};

#endif //ENVIRONMENT_H
