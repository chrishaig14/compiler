//
// Created by chris on 6/4/21.
//
#ifndef CONST_FUNCTION_H
#define CONST_FUNCTION_H

#include "Entity.h"
#include "../ast/Type.h"
#include "../ast/Path.h"
#include "../ast/TypeFunction.h"
#include "../ast/Function.h"

class ConstFunction {
public:
    ConstFunction(Path path, ast::FunctionType* ft);

    ConstFunction(const ConstFunction& other);

    ~ConstFunction();

    Implicit* implicit;
    ast::FunctionType* ft;
    Path path;
};

#endif