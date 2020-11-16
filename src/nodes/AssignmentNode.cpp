//
// Created by chris on 1/8/20.
//

#include "AssignmentNode.h"

void AssignmentNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

AssignmentNode::AssignmentNode(NodeContainer lvalue, NodeContainer rvalue) : lvalue(lvalue), rvalue(rvalue) {}

bool AssignmentNode::operator==(AssignmentNode& other) const {
//    return this->lvalue->equal(other.lvalue) && this->rvalue->equal(other.rvalue);
    return false;
}

AssignmentNode::~AssignmentNode() {
//    delete this->lvalue;
//    delete this->rvalue;
}
