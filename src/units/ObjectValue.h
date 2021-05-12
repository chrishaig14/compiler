//
// Created by chris on 8/4/21.
//

#ifndef XLANG_OBJECTVALUE_H
#define XLANG_OBJECTVALUE_H


#include <cassert>
#include "Entity.h"
#include "../nodes/TypeNode.h"

enum class Meta {
    ENUM, CLASS
};

class Value {

public:
    TypeNode* type;
    union {
        Class* clazz;
        Enum* enumm;
    };
    Meta metatype;

    Value(TypeNode* type) {
        this->clazz = nullptr;
        this->enumm = nullptr;
        assert(type != nullptr);
        this->type = type;
    }
};


#endif //XLANG_OBJECTVALUE_H
