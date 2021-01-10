//
// Created by chris on 25/11/20.
//

#include "FloatNode.h"

bool FloatNode::equal(const Node& other) const {
    return other.flot().value == this->value;
}

FloatNode& FloatNode::flot() {
    return *this;
}

const FloatNode& FloatNode::flot() const {
    return *this;
}
