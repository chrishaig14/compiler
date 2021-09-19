//
// Created by chris on 26/4/21.
//

#include "../include/Break.h"

using namespace sem;

Break::Break() : SNode(SNodeType::BREAK) {
}

bool Break::equals(const SNode& o) const {
    return true;
}
