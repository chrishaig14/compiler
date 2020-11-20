//
// Created by chris on 1/8/20.
//

#include "ReturnNode.h"

ReturnNode::ReturnNode(Node* expression) : expression(expression) {this->ntype = RETRN;}

bool ReturnNode::equal(const Node& x) const {
    return *x.retrn().expression == *this->expression;
}

ReturnNode &ReturnNode::retrn() {
    return *this;
}

const ReturnNode &ReturnNode::retrn() const {
    return *this;
}
