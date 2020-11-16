//
// Created by chris on 17/8/20.
//

#include "BooleanNode.h"

void BooleanNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

BooleanNode::BooleanNode(bool value) : value(value) {}

bool BooleanNode::operator!=(const BooleanNode& other) const {return !(*this==other);}
