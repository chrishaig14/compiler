//
// Created by chris on 1/8/20.
//

#include "MemberNode.h"

MemberNode::MemberNode(Node* parent, const std::string& child) : parent(parent), child(child) { this->ntype = MEMBER; }

MemberNode& MemberNode::member() {
    return *this;
}

const MemberNode& MemberNode::member() const {
    return *this;
}

MemberNode::~MemberNode() {
    delete this->parent;
}

bool MemberNode::equal(const Node& x) const {
    auto& other = x.member();
    return *this->parent == *other.parent && this->child == other.child;
}

json MemberNode::to_json() const {
    json j;
    j["node"] = "member";
//        j["parent"] = this->parent->to_json();
    j["child"] = this->child;
    return j;
}
