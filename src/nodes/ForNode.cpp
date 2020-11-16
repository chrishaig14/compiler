//
// Created by chris on 14/8/20.
//

#include "ForNode.h"

void ForNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

ForNode::ForNode(const std::string& var, NodeContainer exp, BlockNode body) : var(var), exp(exp), body(body) {}

bool ForNode::operator==(const ForNode& other) const {
    return this->var == other.var && this->exp == other.exp && this->body == other.body;
}

bool ForNode::operator!=(const ForNode& other) const {
    return !(*this == other);
}
