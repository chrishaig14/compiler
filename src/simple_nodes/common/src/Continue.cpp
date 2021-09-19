//
// Created by chris on 26/4/21.
//

#include "../include/Continue.h"

using namespace sem;

Continue::Continue() : SNode(SNodeType::CONTINUE) {
}

bool Continue::equals(const SNode& o) const {
    return true;
}
