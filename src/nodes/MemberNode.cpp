//
// Created by chris on 1/8/20.
//

#include "MemberNode.h"

void MemberNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

MemberNode::MemberNode(NodeContainer parent, const std::string& child) : parent(parent), child(child) {}
