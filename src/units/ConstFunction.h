//
// Created by chris on 6/4/21.
//
#ifndef CONST_FUNCTION_H
#define CONST_FUNCTION_H

#include "Entity.h"
#include "../ast/Type.h"
#include "../ast/Path.h"
#include "../ast/FunctionType.h"
#include "../ast/Function.h"
#include "../simple_nodes/sem.h"
#include "../simple_nodes/TypeFunction.h"

class ConstFunction {
public:
    ConstFunction(Path path, sem::TypeFunction* ft);

    ConstFunction(const ConstFunction& other);

    ~ConstFunction();

    Implicit* implicit;
    sem::TypeFunction* const_function_ft_p;
    sem::TypeFunction& const_function_ft;
    Path path;
};

#endif