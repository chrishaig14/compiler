//
// Created by chris on 21/7/20.
//

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include "Object.h"

class Environment {
    std::map<std::string, Object*> table;

    Environment* parent;
    std::string name;
public:
    Environment(std::string name, Environment* parent);

    void set(std::string name, Object* value);

    bool is_declared(std::string name);

    Object* get(std::string name);

    void declare(std::string name);

    Environment* enter(std::string name);

    Environment* leave(std::string name);
};

#endif //ENVIRONMENT_H
