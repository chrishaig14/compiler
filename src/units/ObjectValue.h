//
// Created by chris on 8/4/21.
//

#ifndef XLANG_OBJECTVALUE_H
#define XLANG_OBJECTVALUE_H


#include <cassert>
#include "Entity.h"
#include "../nodes/TypeNode.h"

class Value {

public:
    TypeNode* type;
    Class* clazz;

    Value(TypeNode* type) {
        this->clazz = nullptr;
        assert(type != nullptr);
        this->type = type;
    }
};


#endif //XLANG_OBJECTVALUE_H
