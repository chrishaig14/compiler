//
// Created by chris on 16/8/20.
//

#include "WhileNode.h"
#include "../utils.h"
void WhileNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

WhileNode::WhileNode(NodeContainer condition, BlockNode body) : body(body), condition(condition) {}

bool WhileNode::operator==(const WhileNode& other) const {
    return this->condition==other.condition && this->body == other.body;
}

bool WhileNode::operator!=(const WhileNode& other) const {
    return !(*this==other);
}

