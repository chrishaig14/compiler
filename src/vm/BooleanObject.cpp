//
// Created by chris on 5/8/20.
//

#include "BooleanObject.h"

bool BooleanObject::equal(const Object* other) const {
    const BooleanObject* other_ptr = dynamic_cast<const BooleanObject*>(other);
    return this->boolean == other_ptr->boolean;
}

BooleanObject::BooleanObject(bool boolean) {
    this->boolean = boolean;
}
