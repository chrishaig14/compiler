//
// Created by chris on 1/8/20.
//

#include "BinopNode.h"

void BinopNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

BinopNode::BinopNode(OpType op, Node* left, Node* right) : left(left), right(right), op(op) {this->ntype =BINOP;}

bool BinopNode::equal(const Node& x) const {
    auto& other = x.binop();
    return this->op == other.op && *this->left == *other.left and *this->right == *other.right;
}


