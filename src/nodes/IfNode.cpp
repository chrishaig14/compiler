//
// Created by chris on 1/8/20.
//

#include "IfNode.h"
#include "../utils.h"
void IfNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool IfNode::equal(NodeContainer other) const {
//    auto other_ptr = dynamic_cast<IfNode*>(other);
//    if (other_ptr == nullptr) return false;
//    return *this == *other_ptr;
return false;
}

bool IfNode::operator==(IfNode& other) const {
    return this->condition==other.condition && this->then == other.then;
}

IfNode::IfNode(NodeContainer condition, BlockNode then,
               const std::vector<std::pair<NodeContainer, std::reference_wrapper<BlockNode>>>& elifs, BlockNode selse) : condition(condition),
                                                                                            then(then), selse(selse),
                                                                                            elifs(elifs) {}

