//
// Created by chris on 1/8/20.
//

#include "AssignmentNode.h"

AssignmentNode::AssignmentNode(Node* lvalue, Node* rvalue) : lvalue(lvalue), rvalue(rvalue) {this->ntype =ASSIGN;}

bool AssignmentNode::equal(const Node& x) const {
    auto& other = x.assign();
    return *this->lvalue == *other.lvalue && *this->rvalue == *other.rvalue;
}

AssignmentNode::~AssignmentNode() {
    delete this->lvalue;
    delete this->rvalue;
}
