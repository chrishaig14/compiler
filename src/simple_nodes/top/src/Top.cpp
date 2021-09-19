//
// Created by chris on 4/4/21.
//

#include "../include/Top.h"

using namespace sem;

Top::Top(TopType type) {
    this->type = type;
}

bool Top::operator!=(const Top& other) const {
    return !(*this == other);
}

bool Top::operator==(const Top& other) const {
    if (this->type != other.type) {
        return false;
    }
    return this->equals(other);
}
