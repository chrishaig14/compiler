//
// Created by chris on 1/8/20.
//

#include "BinopNode.h"

void BinopNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

BinopNode::BinopNode(OpType op, Node* left, Node* right) : left(left), right(right), op(op) {}
