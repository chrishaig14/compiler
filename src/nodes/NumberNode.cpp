//
// Created by chris on 1/8/20.
//

#include "NumberNode.h"

void NumberNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

NumberNode::NumberNode(int number) : number(number) {}

bool NumberNode::operator==(const NumberNode& other) const {
    return this->number == other.number;
}

bool NumberNode::operator!=(const NumberNode& other) const {
    return !(*this == other);
}
