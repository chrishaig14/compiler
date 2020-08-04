//
// Created by chris on 3/8/20.
//

#ifndef UNTITLED1_INTEGEROBJECT_H
#define UNTITLED1_INTEGEROBJECT_H

#include "Object.h"

class IntegerObject : public Object {
public:
    IntegerObject(int value);

    IntegerObject();

    bool operator==(const IntegerObject& other) const {
        return this->value == other.value;
    }

    bool equal(const Object* other) const;

    Object* sum(const Object* other) const override {
        const IntegerObject* other_ptr = dynamic_cast<const IntegerObject*>(other);
        return new IntegerObject(this->value + other_ptr->value);
    }

    int value;
};


#endif //UNTITLED1_INTEGEROBJECT_H
