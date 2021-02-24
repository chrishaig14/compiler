//
// Created by chris on 18/8/20.
//

#include "BreakNode.h"

bool BreakNode::equal(const Node& other) const {
    return true;
}

BreakNode::BreakNode() { this->ntype = BRK; }

BreakNode& BreakNode::brk() {
    return *this;
}

const BreakNode& BreakNode::brk() const {
    return *this;
}

