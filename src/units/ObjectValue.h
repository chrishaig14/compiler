//
// Created by chris on 8/4/21.
//

#ifndef XLANG_OBJECTVALUE_H
#define XLANG_OBJECTVALUE_H


#include "Entity.h"
#include "../nodes/TypeNode.h"
class ObjectValue : public Entity {

public:
    ObjectValue() : Entity(E_TYPE::OBJECT_VALUE) {
    }

    ObjectType* ot;
};


#endif //XLANG_OBJECTVALUE_H
