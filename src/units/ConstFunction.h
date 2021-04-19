//
// Created by chris on 6/4/21.
//
#ifndef CONST_FUNCTION_H
#define CONST_FUNCTION_H

#include "Entity.h"
#include "../nodes/TypeNode.h"

class ConstFunction {
public:
    ConstFunction(){}
    ConstFunction(const ConstFunction& other){
        this->ft = other.ft->clone();
    }

    ~ConstFunction() {
        std::cout << "Destroyinh CONST FUnction!" << std::endl;
    }

    FunctionType* ft;
    Path path;
};

#endif