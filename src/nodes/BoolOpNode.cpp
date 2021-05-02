//
// Created by chris on 25/11/20.
//

#include "BoolOpNode.h"


BoolOpNode::BoolOpNode(BoolOp op, Node* left, Node* right, TextPosition start, TextPosition end)
        :Node(NodeType::BOOLOP, start, end), left(left), right(right), op(op) {
}

bool BoolOpNode::equal(const Node& x) const {
    auto& other = x.boolop();
    return this->op == other.op && *this->left == *other.left and *this->right == *other.right;
}

BoolOpNode::~BoolOpNode() {
    delete this->left;
    delete this->right;
}

BoolOpNode& BoolOpNode::boolop() { return *this; }

const BoolOpNode& BoolOpNode::boolop() const { return *this; }
