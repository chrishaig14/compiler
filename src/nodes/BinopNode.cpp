//
// Created by chris on 1/8/20.
//

#include "BinopNode.h"

BinopNode::BinopNode(OpType op, Node* left, Node* right, TextPosition start, TextPosition end)
        : Node(NodeType::BINOP, start, end),left(left), right(right), op(op) {
}

bool BinopNode::equal(const Node& x) const {
    auto& other = x.binop();
    return this->op == other.op && *this->left == *other.left and *this->right == *other.right;
}

json BinopNode::to_json() const {
    json j;
    j["node"] = "binop";
//        j["left"] = this->left->to_json();
//        j["right"] = this->right->to_json();
    j["op"] = op_to_string(this->op);
    return j;
}

BinopNode::~BinopNode() {
    delete this->left;
    delete this->right;
}

BinopNode& BinopNode::binop() { return *this; }

const BinopNode& BinopNode::binop() const { return *this; }


