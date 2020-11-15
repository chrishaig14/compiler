//
// Created by chris on 16/8/20.
//

#include "WhileNode.h"

void WhileNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool WhileNode::equal(NodeContainer other) const {
//    WhileNode* other_ptr = dynamic_cast<WhileNode*>(other);
//    if (other_ptr == nullptr) return false;
//    return *this == *other_ptr;
    return false;
}

WhileNode::WhileNode(NodeContainer condition, BlockNode body) : body(body), condition(condition) {}

bool WhileNode::operator==(const WhileNode& other) const {
//    return this->condition->equal(other.condition) && this->body->equal(other.body);
    return false;
}

