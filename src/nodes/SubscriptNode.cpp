//
// Created by chris on 1/8/20.
//

#include "SubscriptNode.h"

SubscriptNode::SubscriptNode(Node* parent, VectorOfNodes child, TextPosition start, TextPosition end) : Node(NodeType::SUB,
                                                                                                             start,
                                                                                                             end),
                                                                                                        parent(parent),
                                                                                                        child(child) {
}


bool SubscriptNode::equal(const Node& x) const {
    auto& other = x.sub();
    if (this->child.size() != other.child.size()) {
        return false;
    }
    for (size_t i = 0; i < this->child.size(); i++) {
        if (*this->child[i] != *other.child[i]) {
            return false;
        }
    }
    return *this->parent == *other.parent;
}

SubscriptNode& SubscriptNode::sub() {
    return *this;
}

const SubscriptNode& SubscriptNode::sub() const {
    return *this;
}

SubscriptNode::~SubscriptNode() {
    delete this->parent;
    for (auto c: this->child) {
        delete c;
    }
}

