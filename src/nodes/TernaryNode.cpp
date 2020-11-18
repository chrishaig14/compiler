//
// Created by chris on 29/8/20.
//

#include "TernaryNode.h"
#include "../utils.h"
void TernaryNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool TernaryNode::operator==(const TernaryNode& other) const {
    return this->expression == other.expression && this->true_case == other.true_case && this->false_case == other.false_case;
}

TernaryNode::TernaryNode(Node* expression, Node* trueCase, Node* falseCase) : expression(
        expression),
                                                                                                      true_case(
                                                                                                              trueCase),
                                                                                                      false_case(
                                                                                                              falseCase) {}
