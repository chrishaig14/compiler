//
// Created by chris on 23/11/20.
//

#include "TupleObject.h"

TupleObject::TupleObject(std::vector<Object*> values) {
    this->values = values;
}

bool TupleObject::equal(const Object* other) const {
    return false;
}
