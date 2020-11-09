//
// Created by chris on 29/8/20.
//

#include "TernaryNode.h"

void TernaryNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool TernaryNode::equal(NodeContainer other) const {
//    auto other_ptr = dynamic_cast<TernaryNode*>(other);
//    if (other_ptr == nullptr) return false;
//    return *this == *other_ptr;
    return false;
}

bool TernaryNode::operator==(TernaryNode& other) const {
//    if (!this->expression->equal(other.expression)) return false;
//    if (!this->true_case->equal(other.true_case)) return false;
//    if (!this->false_case->equal(other.false_case)) return false;
//    return true;
    return false;
}

TernaryNode::TernaryNode(NodeContainer expression, NodeContainer trueCase, NodeContainer falseCase) : expression(
        expression),
                                                                                                      true_case(
                                                                                                              trueCase),
                                                                                                      false_case(
                                                                                                              falseCase) {}
