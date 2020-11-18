//
// Created by chris on 1/8/20.
//

#include "SubscriptNode.h"

void SubscriptNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

SubscriptNode::SubscriptNode(Node* parent, VectorOfNodes child) : parent(parent), child(child) {this->ntype = SUB;}


bool SubscriptNode::equal(const Node& x) const {
    auto& other = x.sub();
    if (this->child.size() != other.child.size()) {
        return false;
    }
    for (int i = 0; i < this->child.size(); i++) {
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
