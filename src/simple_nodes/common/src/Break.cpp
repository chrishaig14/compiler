//
// Created by chris on 26/4/21.
//

#include "../include/Break.h"

using namespace sem;

Break::Break() : Common(CommonType::BREAK) {
}

bool Break::equals(const Common& o) const {
    return true;
}

std::unique_ptr<Common> Break::clone() const {
    return std::make_unique<Break>(*this);
}
