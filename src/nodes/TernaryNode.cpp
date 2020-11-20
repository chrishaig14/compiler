//
// Created by chris on 29/8/20.
//

#include "TernaryNode.h"
#include "../utils.h"

bool TernaryNode::equal(const Node& x) const {
    auto& other = x.ternary();
    return *this->expression == *other.expression && *this->true_case == *other.true_case &&
           *this->false_case == *other.false_case;
}

TernaryNode::TernaryNode(Node* expression, Node* trueCase, Node* falseCase) : expression(
        expression),
                                                                              true_case(
                                                                                      trueCase),
                                                                              false_case(
                                                                                      falseCase) {this->ntype = TERNARY;}

TernaryNode &TernaryNode::ternary() {
    return *this;
}

const TernaryNode &TernaryNode::ternary() const {
    return *this;
}

TernaryNode::~TernaryNode() {
    delete expression;
    delete true_case;
    delete false_case;
}
