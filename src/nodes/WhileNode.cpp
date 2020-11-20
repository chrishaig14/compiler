//
// Created by chris on 16/8/20.
//

#include "WhileNode.h"

WhileNode::WhileNode(Node* condition, BlockNode* body) : body(body), condition(condition) { this->ntype = WHIL; }

bool WhileNode::equal(const Node& x) const {
    auto& other = x.whil();
    if (this->body == nullptr && other.body != nullptr || this->body != nullptr && other.body == nullptr) {
        return false;
    }

    return *this->condition == *other.condition &&
           (this->body == nullptr && other.body == nullptr || *this->body == *other.body);
}

WhileNode& WhileNode::whil() {
    return *this;
}

const WhileNode& WhileNode::whil() const {
    return *this;
}

WhileNode::~WhileNode() {
    delete this->body;
    delete this->condition;
}


