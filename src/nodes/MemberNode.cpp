//
// Created by chris on 1/8/20.
//

#include "MemberNode.h"

void MemberNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

MemberNode::MemberNode(Node* parent, const std::string& child) : parent(parent), child(child) { this->ntype = MEMBER; }

MemberNode& MemberNode::member() {
    return *this;
}

const MemberNode& MemberNode::member() const {
    return *this;
}
