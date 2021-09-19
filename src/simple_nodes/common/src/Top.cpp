//
// Created by chris on 4/4/21.
//

#include "../include/Common.h"
using namespace sem;
Common::Common(CommonType type) {
    this->type = type;
}

bool Common::operator!=(const Common& other) const {
    return !(*this == other);
}

bool Common::operator==(const Common& other) const {
    if (this->type != other.type) {
        return false;
    }
    return this->equals(other);
}
