//
// Created by chris on 12/4/21.
//

#include "../include/Union.h"

using namespace sem;

bool Union::equals(const Exp& o) const {
    return true;
}

UExp Union::clone() const {
    return std::make_unique<Union>(*this);
}
