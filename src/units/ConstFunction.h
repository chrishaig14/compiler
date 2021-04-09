//
// Created by chris on 6/4/21.
//
#ifndef CONST_FUNCTION_H
#define CONST_FUNCTION_H

#include "Entity.h"
#include "../nodes/TypeNode.h"

class ConstFunction : public Entity {
public:
    ConstFunction() : Entity(E_TYPE::CONST_FUNCTION) {
    }

    FunctionType* ft;
};

#endif