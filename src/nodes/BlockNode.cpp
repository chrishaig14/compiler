//
// Created by chris on 2/8/20.
//

#include "BlockNode.h"

void BlockNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

BlockNode& BlockNode::block() {
    return *this;
}

const BlockNode& BlockNode::block() const {
    return *this;
}

