//
// Created by chris on 25/11/20.
//

#include "BoolOpNode.h"


BoolOpNode::BoolOpNode(BoolOp op, Node* left, Node* right) : left(left), right(right), op(op) { this->ntype = BOOLOP; }

bool BoolOpNode::equal(const Node& x) const {
    auto& other = x.boolop();
    return this->op == other.op && *this->left == *other.left and *this->right == *other.right;
}

json BoolOpNode::to_json() const {
    json j;
    j["node"] = "binop";
//        j["left"] = this->left->to_json();
//        j["right"] = this->right->to_json();
    j["op"] = bool_op_to_string(this->op);
    return j;
}

BoolOpNode::~BoolOpNode() {
    delete this->left;
    delete this->right;
}

BoolOpNode& BoolOpNode::boolop() { return *this; }

const BoolOpNode& BoolOpNode::boolop() const { return *this; }
