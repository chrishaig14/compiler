//
// Created by chris on 14/8/20.
//

#include "ForNode.h"

void ForNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool ForNode::equal(NodeContainer other) const {
//    const ForNode* other_ptr = dynamic_cast<const ForNode*>(other);
//    if (other_ptr == nullptr) return false;
//    return this->var == other_ptr->var && this->exp->equal(other_ptr->exp) && this->body->equal(other_ptr->body);
    return false;
}

ForNode::ForNode(const std::string& var, NodeContainer exp, BlockNode body) : var(var), exp(exp), body(body) {}
