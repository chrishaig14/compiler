//
// Created by chris on 30/8/20.
//

#include "NoneNode.h"

void NoneNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool NoneNode::equal(const Node& other) const {
    return true;
}

