//
// Created by chris on 1/8/20.
//

#include "SubscriptNode.h"

void SubscriptNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

SubscriptNode::SubscriptNode(Node* parent, Node* child) : parent(parent), child(child) {}

bool SubscriptNode::equal(Node* other) const {
    auto other_ptr = dynamic_cast<SubscriptNode*>(other);
    if (other_ptr == nullptr) return false;
    return *this == *other_ptr;
}

bool SubscriptNode::operator==(SubscriptNode& other) const {
    return this->parent->equal(other.parent) and this->child->equal(other.child);
}
