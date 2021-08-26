//
// Created by chris on 6/4/21.
//
#ifndef CONST_FUNCTION_H
#define CONST_FUNCTION_H

#include "Entity.h"
#include "../ast/TypeNode.h"
#include "../ast/Path.h"
#include "../ast/FunctionType.h"
#include "../ast/Function.h"

class ConstFunction {
public:
    ConstFunction(Path path, FunctionType* ft) {
        this->path = path;
        this->ft = ft;
        this->implicit = nullptr;
    }

    ConstFunction(const ConstFunction& other) {
        this->ft = other.ft->clone();
        this->implicit = other.implicit;
    }

    ~ConstFunction() {
        std::cout << "Destroyinh CONST FUnction!" << std::endl;
    }

    Implicit* implicit;
    FunctionType* ft;
    Path path;
};

#endif