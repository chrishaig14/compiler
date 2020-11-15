//
// Created by chris on 17/8/20.
//

#include "BooleanNode.h"

void BooleanNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool BooleanNode::equal(NodeContainer other) const {
//    BooleanNode* other_ptr = dynamic_cast<BooleanNode*>(other);
//    if (other_ptr == nullptr) return false;
//    return *this == *other_ptr;
return false;
}

BooleanNode::BooleanNode(bool value) : value(value) {}

bool BooleanNode::operator!=(const BooleanNode& other) const {return !(*this==other);}
