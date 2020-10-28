//
// Created by chris on 1/8/20.
//

#include "IfNode.h"

void IfNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool IfNode::equal(Node* other) const {
    auto other_ptr = dynamic_cast<IfNode*>(other);
    if (other_ptr == nullptr) return false;
    return *this == *other_ptr;
}

bool IfNode::operator==(IfNode& other) const {
    return this->condition->equal(other.condition);
}

IfNode::IfNode(Node* condition, BlockNode* then,
               const std::vector<std::pair<Node*, BlockNode*>>& elifs, BlockNode* selse) : condition(condition),
                                                                                            then(then), selse(selse),
                                                                                            elifs(elifs) {}

