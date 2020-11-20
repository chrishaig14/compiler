//
// Created by chris on 3/8/20.
//

#include "StringObject.h"

bool StringObject::equal(const Object* other) const {
    const StringObject* other_ptr = dynamic_cast<const StringObject*>(other);
    if (other_ptr == nullptr) {
        return false;
    }
    return this->str == other_ptr->str;
}

StringObject::StringObject(std::string str) {
    this->str = str;
}
