//
// Created by chris on 24/3/21.
//

#include "MethodNode.h"

MethodNode::MethodNode(Node* parent, const std::string& child, TextPosition start) : parent(parent), s_child(child) {
    this->ntype = METHOD;
    this->type = MethodType::STR;
    this->start = start;
}

MethodNode::MethodNode(Node* parent, int child, TextPosition start) : parent(parent), n_child(child) {
    this->ntype = METHOD;
    this->type = MethodType::NUM;
    this->start = start;
}


MethodNode& MethodNode::method() {
    return *this;
}

const MethodNode& MethodNode::method() const {
    return *this;
}

MethodNode::~MethodNode() {
    delete this->parent;
}

bool MethodNode::equal(const Node& x) const {
    auto& other = x.method();
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

json MethodNode::to_json() const {
    json j;
    j["node"] = "Method";
//        j["parent"] = this->parent->to_json();
    j["child"] = this->s_child;
    return j;
}
