//
// Created by chris on 1/8/20.
//

#include "MemberNode.h"

void MemberNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

MemberNode::MemberNode(Node* parent, const std::string& child) : parent(parent), child(child) {}
