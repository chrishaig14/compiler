//
// Created by chris on 5/8/20.
//

#ifndef BOOLEANOBJECT_H
#define BOOLEANOBJECT_H


#include "Object.h"

class BooleanObject : public Object {

public:
    bool equal(const Object* other) const override;

    bool boolean;

    BooleanObject(bool boolean);
};


#endif //BOOLEANOBJECT_H
