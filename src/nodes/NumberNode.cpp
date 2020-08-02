//
// Created by chris on 1/8/20.
//

#include "NumberNode.h"

void NumberNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

NumberNode::NumberNode(int number) : number(number) {}

bool NumberNode::equal(Node* other) const {
    auto other_ptr = dynamic_cast<NumberNode*>(other);
    if (other_ptr == nullptr) return false;
    return *this == *other_ptr;
}

bool NumberNode::operator==(NumberNode& other) const {
    return this->number == other.number;
}
