//
// Created by chris on 1/8/20.
//

#include "MemberNode.h"

MemberNode::MemberNode(Node* parent, const std::string& child, TextPosition start, TextPosition end) : Node(MEMBER, start, end),parent(parent), s_child(child) {
    this->type = MemberType::STR;
}

MemberNode::MemberNode(Node* parent, int child, TextPosition start, TextPosition end) :Node(MEMBER, start, end), parent(parent), n_child(child) {
    this->type = MemberType::NUM;
}


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
    if (this->type != other.type) {
        return false;
    }
    if (this->type == MemberType::NUM) {
        if (this->n_child != other.n_child) {
            return false;
        }
    } else {
        if (this->s_child != other.s_child) {
            return false;
        }
    };
    return *this->parent == *other.parent;
}

json MemberNode::to_json() const {
    json j;
    j["node"] = "member";
//        j["parent"] = this->parent->to_json();
    j["child"] = this->s_child;
    return j;
}
