//
// Created by chris on 3/8/20.
//

#include "IntegerObject.h"

IntegerObject::IntegerObject() {}

IntegerObject::IntegerObject(int value) {
    this->value = value;
}

bool IntegerObject::equal(const Object* other) const {
    const IntegerObject* other_ptr = dynamic_cast<const IntegerObject*>(other);
    if (other_ptr == nullptr) {
        return false;
    }
    return this->value == other_ptr->value;
}

bool IntegerObject::operator==(const IntegerObject& other) const {
    return this->value == other.value;
}

Object* IntegerObject::sum(const Object* other) const {
    const IntegerObject* other_ptr = dynamic_cast<const IntegerObject*>(other);
    return new IntegerObject(this->value + other_ptr->value);
}
