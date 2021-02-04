//
// Created by chris on 3/8/20.
//

#ifndef INTEGEROBJECT_H
#define INTEGEROBJECT_H

#include "Object.h"

class IntegerObject : public Object {
public:
    IntegerObject(int value);

    IntegerObject();

    bool operator==(const IntegerObject& other) const;

    bool equal(const Object* other) const;

    Object* sum(const Object* other) const override;
    int value;
};


#endif //INTEGEROBJECT_H
