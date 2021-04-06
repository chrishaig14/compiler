//
// Created by chris on 17/8/20.
//

#include "BooleanNode.h"

BooleanNode::BooleanNode(bool value, TextPosition start, TextPosition end) : Node(NodeType::BOOLEAN, start, end),value(value) { }

bool BooleanNode::operator!=(const BooleanNode& other) const { return !(*this == other); }

bool BooleanNode::equal(const Node& x) const {
    auto& other = x.boolean();
    return this->value == other.value;
}

BooleanNode& BooleanNode::boolean() {
    return *this;
}

const BooleanNode& BooleanNode::boolean() const {
    return *this;
}
