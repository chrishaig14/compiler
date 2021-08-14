//
// Created by chris on 1/8/20.
//

#include "MemberNode.h"

MemberNode::MemberNode(Node* parent, Token child_token) : Node(NodeType::MEMBER, parent->start, child_token.end_pos),
                                                          parent(parent) {
    this->child_token = child_token;
    this->type = MemberType::STR;
    this->s_child = child_token.str;
}

MemberNode::~MemberNode() {
    // delete this->parent;
}

bool MemberNode::equal(const Node& x) const {
    auto& other = (MemberNode&) x;
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

nlohmann::json MemberNode::to_json() const {
    return {{"type",   "member"},
            {"member", {{"parent", this->parent->to_json()}, {"child", this->s_child}}}};
}

