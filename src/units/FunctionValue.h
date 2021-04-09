//
// Created by chris on 8/4/21.
//

#ifndef XLANG_FUNCTIONVALUE_H
#define XLANG_FUNCTIONVALUE_H


#include "Entity.h"
#include "../nodes/TypeNode.h"
class FunctionValue : public Entity {
public:
    FunctionValue():Entity(E_TYPE::FUNCTION_VALUE){}

    FunctionType* ft;
};


#endif //XLANG_FUNCTIONVALUE_H
