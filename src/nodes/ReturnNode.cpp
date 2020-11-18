//
// Created by chris on 1/8/20.
//

#include "ReturnNode.h"

void ReturnNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

ReturnNode::ReturnNode(Node* expression) : expression(expression) {}

bool ReturnNode::equal(const Node& x) const {
    return *x.retrn().expression == *this->expression;
}