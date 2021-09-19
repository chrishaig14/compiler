//
// Created by chris on 18/9/21.
//

#include "../include/Exp.h"

bool sem::Exp::operator==(const sem::Exp& other) const {
    if (this->type != other.type) {
        return false;
    }
    return this->equals(other);
}

bool sem::Exp::operator!=(const sem::Exp& other) const {
    return !(*this == other);
}

sem::Exp::Exp(sem::ExpType type) : type(type) {
}
