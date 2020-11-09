//
// Created by chris on 1/8/20.
//

#include "SubscriptNode.h"

void SubscriptNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

SubscriptNode::SubscriptNode(NodeContainer parent, std::vector<NodeContainer> child) : parent(parent), child(child) {}

bool SubscriptNode::equal(NodeContainer other) const {
//    auto other_ptr = dynamic_cast<SubscriptNode*>(other);
//    if (other_ptr == nullptr) return false;
//    return *this == *other_ptr;
return false;
}

bool SubscriptNode::operator==(SubscriptNode& other) const {
    if (this->child.size() != other.child.size())return false;
    for (int i = 0; i < this->child.size(); i++) {
//        if (!this->child[i]->equal(other.child[i])) return false;
    }
//    return this->parent->equal(other.parent);
return false;
}
