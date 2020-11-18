//
// Created by chris on 1/8/20.
//

#include "IfNode.h"
#include "../utils.h"

void IfNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool IfNode::equal(const Node& x) const {
    auto& other = x.iff();
    return this->condition == other.condition && this->then == other.then;
}

IfNode::IfNode(Node* condition, BlockNode* then,
               std::vector<std::pair<Node*, BlockNode*>> elifs, BlockNode* selse) : condition(condition),
                                                                                    then(then), selse(selse),
                                                                                    elifs(elifs) {}

