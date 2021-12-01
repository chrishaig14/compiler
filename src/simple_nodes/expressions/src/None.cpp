//
// Created by chris on 8/5/21.
//

#include "../include/None.h"

using namespace sem;

None::None() : Exp(ExpType::NONE) {
}

bool None::equals(const Exp& o) const {
    return true;
}

UExp None::clone() const {
    return std::make_unique<None>();
}
