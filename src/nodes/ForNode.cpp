//
// Created by chris on 14/8/20.
//

#include <cassert>
#include "ForNode.h"

ForNode::ForNode(const std::string& var, Node* exp, BlockNode* body, TextPosition start, TextPosition end) : Node(
        NodeType::FORLOOP,
        start,
        end), var(var), exp(exp), body(body) {
    assert(exp != nullptr);
    assert(body != nullptr);
}

bool ForNode::equal(const Node& x) const {
    const auto& other = (ForNode&) x;
    return this->var == other.var && *this->exp == *other.exp && *this->body == *other.body;
}

ForNode::~ForNode() {
    delete this->exp;
    delete this->body;
}

nlohmann::json ForNode::to_json() {
    nlohmann::json j;
    j["type"] = "for";
    j["for"]["var"] = this->var;
    j["for"]["exp"] = this->exp->to_json();
    j["for"]["body"] = this->body->to_json();
    return j;
}

