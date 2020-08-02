//
// Created by chris on 1/8/20.
//

#include "AssignmentNode.h"

void AssignmentNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

AssignmentNode::AssignmentNode(Node* lvalue, Node* rvalue) : lvalue(lvalue), rvalue(rvalue) {}

bool AssignmentNode::equal(Node* other) const {
    auto other_ptr = dynamic_cast<AssignmentNode*>(other);
    if (other_ptr == nullptr) return false;
    return *this == *other_ptr;
}

bool AssignmentNode::operator==(AssignmentNode& other) const {
    return this->lvalue->equal(other.lvalue) && this->rvalue->equal(other.rvalue);
}
