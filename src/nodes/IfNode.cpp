//
// Created by chris on 1/8/20.
//

#include "IfNode.h"

void IfNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

IfNode::IfNode(Node* condition, std::vector<Node*> then) : condition(condition), then(then) {}

bool IfNode::equal(Node* other) const {
    auto other_ptr = dynamic_cast<IfNode*>(other);
    if (other_ptr == nullptr) return false;
    return *this == *other_ptr;
}

bool IfNode::operator==(IfNode& other) const {
    return this->condition->equal(other.condition);
}