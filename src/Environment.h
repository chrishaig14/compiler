//
// Created by chris on 21/7/20.
//

#ifndef UNTITLED1_ENVIRONMENT_H
#define UNTITLED1_ENVIRONMENT_H

#include <map>
#include "Value.h"

class Environment {
    std::map<std::string, Value*> table;

    Environment* parent;
public:
    Environment(Environment* parent);

    void set(std::string name, Value* value);

    Value* get(std::string name);

    void declare(std::string name);

    Environment* enter();

    Environment* leave();
};

#endif //UNTITLED1_ENVIRONMENT_H
