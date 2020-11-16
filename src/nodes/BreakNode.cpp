//
// Created by chris on 18/8/20.
//

#include "BreakNode.h"

void BreakNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool BreakNode::operator==(const BreakNode& other) const {
    return true;
}

bool BreakNode::operator!=(const BreakNode& other) const {
    return false;
}
