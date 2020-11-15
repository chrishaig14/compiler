//
// Created by chris on 1/8/20.
//

#include "NumberNode.h"

void NumberNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

NumberNode::NumberNode(int number) : number(number) {}

bool NumberNode::equal(NodeContainer other) const {
//    auto other_ptr = dynamic_cast<NumberNode*>(other);
//    if (other_ptr == nullptr) return false;
//    return *this == *other_ptr;
return false;
}

bool NumberNode::operator==(const NumberNode& other) const {
    return this->number == other.number;
}

bool NumberNode::operator!=(const NumberNode& other) const {
    return !(*this == other);
}
