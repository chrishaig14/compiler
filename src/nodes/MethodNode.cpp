//
// Created by chris on 24/3/21.
//

#include "MethodNode.h"

MethodNode::MethodNode(Node* parent, const std::string& child, TextPosition start, TextPosition end) : Node(NodeType::METHOD, start, end), parent(parent), s_child(child) {
    this->type = MethodType::STR;
}

MethodNode::MethodNode(Node* parent, int child, TextPosition start, TextPosition end) : Node(NodeType::METHOD, start, end), parent(parent), n_child(child) {
    this->type = MethodType::NUM;
}


MethodNode::~MethodNode() {
    delete this->parent;
}

bool MethodNode::equal(const Node& x) const {
    auto& other = (MethodNode&)x;
    if (this->type != other.type) {
        return false;
    }
    if (this->type == MethodType::NUM) {
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

