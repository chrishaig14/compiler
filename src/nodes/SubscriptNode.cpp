//
// Created by chris on 1/8/20.
//

#include "SubscriptNode.h"

void SubscriptNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

SubscriptNode::SubscriptNode(Node* parent, VectorOfNodes child) : parent(parent), child(child) {}


bool SubscriptNode::operator==(const SubscriptNode& other) const {
    if (this->child.size() != other.child.size())return false;
    for (int i = 0; i < this->child.size(); i++) {
//        if (!this->child[i]->equal(other.child[i])) return false;
    }
//    return this->parent->equal(other.parent);
return false;
}

bool SubscriptNode::operator!=(const SubscriptNode& other) const {
    return !(*this==other);
}