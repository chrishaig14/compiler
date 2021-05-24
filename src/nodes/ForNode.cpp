//
// Created by chris on 14/8/20.
//

#include <cassert>
#include "ForNode.h"

ForNode::ForNode(const std::string& var, Node* exp, BlockNode* body, TextPosition start, TextPosition end) :Node(NodeType::FORLOOP, start, end), var(var), exp(exp), body(body) {
    assert(exp != nullptr);
    assert(body != nullptr);
}

bool ForNode::equal(const Node& x) const {
    const auto& other = x.forloop();
    return this->var == other.var && *this->exp == *other.exp && *this->body == *other.body;
}

ForNode& ForNode::forloop() {
    return *this;
}

const ForNode& ForNode::forloop() const {
    return *this;
}

ForNode::~ForNode() {

        delete this->exp;


        delete this->body;

}

