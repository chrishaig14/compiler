//
// Created by chris on 30/8/20.
//

#ifndef UNTITLED1_NONEOBJECT_H
#define UNTITLED1_NONEOBJECT_H


#include "Object.h"

class NoneObject : public Object{
public:
    bool equal(const Object* other) const override;

};


#endif //UNTITLED1_NONEOBJECT_H
