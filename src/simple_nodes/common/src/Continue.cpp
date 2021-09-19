//
// Created by chris on 26/4/21.
//

#include "../include/Continue.h"

using namespace sem;

Continue::Continue() : Common(CommonType::CONTINUE) {
}

bool Continue::equals(const Common& o) const {
    return true;
}
