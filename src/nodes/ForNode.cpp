//
// Created by chris on 14/8/20.
//

#include "ForNode.h"

ForNode::ForNode(const std::string& var, Node* exp, BlockNode* body) : var(var), exp(exp), body(body) {
    assert(exp != nullptr);
    assert(body != nullptr);
    this->ntype = FORLOOP;
}

bool ForNode::equal(const Node& x) const {
    auto& other = x.forloop();
    return this->var == other.var && *this->exp == *other.exp && *this->body == *other.body;
}

ForNode& ForNode::forloop() {
    return *this;
}

const ForNode& ForNode::forloop() const {
    return *this;
}

ForNode::~ForNode() {
    if (this->exp != nullptr) {
        delete this->exp;
    }
    if (this->body != nullptr) {
        delete this->body;
    }
}

