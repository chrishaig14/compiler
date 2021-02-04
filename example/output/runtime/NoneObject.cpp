//
// Created by chris on 30/8/20.
//

#include "NoneObject.h"

bool NoneObject::equal(const Object* other) const {
    auto ptr = dynamic_cast<const NoneObject*>(other);
    return ptr != nullptr;
}
