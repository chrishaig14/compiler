//
// Created by chris on 5/8/20.
//

#ifndef BOOLEANOBJECT_H
#define BOOLEANOBJECT_H


#include "Object.h"

class BooleanObject : public Object {

public:
    bool equal(const Object* other) const override {
        const BooleanObject* other_ptr = dynamic_cast<const BooleanObject*>(other);
        return this->boolean == other_ptr->boolean;
    }

    bool boolean;

    BooleanObject(bool boolean) {
        this->boolean = boolean;
    }
};


#endif //BOOLEANOBJECT_H
