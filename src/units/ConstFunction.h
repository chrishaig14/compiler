//
// Created by chris on 6/4/21.
//
#ifndef CONST_FUNCTION_H
#define CONST_FUNCTION_H

#include "Entity.h"
#include "../nodes/TypeNode.h"
#include "../nodes/Path.h"
#include "../nodes/FunctionType.h"
#include "../nodes/FunctionNode.h"

class ConstFunction {
public:
    ConstFunction() {
        this->ft = nullptr;
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