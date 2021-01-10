//
// Created by chris on 25/11/20.
//

#ifndef UNTITLED1_FLOATOBJECT_H
#define UNTITLED1_FLOATOBJECT_H


#include "Object.h"

class FloatObject : public Object {
public:
    bool equal(const Object* other) const override;

    FloatObject(float f) { this->value = f; }

    float value;
};


#endif //UNTITLED1_FLOATOBJECT_H
