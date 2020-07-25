//
// Created by chris on 21/7/20.
//

#ifndef UNTITLED1_ENVIRONMENT_H
#define UNTITLED1_ENVIRONMENT_H

#include <map>
#include "Object.h"

class Environment {
    std::map<std::string, Object*> table;

    Environment* parent;
public:
    Environment(Environment* parent);

    void set(std::string name, Object* value);

    bool is_declared(std::string name) {
        return this->table.count(name);
    }

    Object* get(std::string name);

    void declare(std::string name);

    Environment* enter();

    Environment* leave();
};

#endif //UNTITLED1_ENVIRONMENT_H
