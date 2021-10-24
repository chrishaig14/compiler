//
// Created by chris on 6/4/21.
//
#ifndef CONST_FUNCTION_H
#define CONST_FUNCTION_H

#include "../entities/Entity.h"
#include <ast/general/Type.h>
#include <common/Path.h>
#include <ast/general/TypeFunction.h>
#include <ast/top/Function.h>
#include <simple_nodes/sem.h>
#include <simple_nodes/common/include/TypeFunction.h>
#include <unordered_map>

class ConstFunction {
private:
    sem::UTypeFunction const_function_ft_p;
public:
    std::unordered_map<std::string, std::string> constraints;

    ConstFunction(Path path, sem::UTypeFunction ft);

    // ConstFunction(const ConstFunction& other);

    ~ConstFunction();
    sem::TypeFunction& const_function_ft;
    Path path;
};

#endif