//
// Created by chris on 30/8/20.
//

#ifndef NONEOBJECT_H
#define NONEOBJECT_H


#include "Object.h"

class NoneObject : public Object {
public:
    bool equal(const Object* other) const override;

};


#endif //NONEOBJECT_H
