//
// Created by chris on 1/8/20.
//

#include "BinopNode.h"

BinopNode::BinopNode(OpType op, std::unique_ptr<Node>& left, std::unique_ptr<Node>& right, TextPosition start,
                     TextPosition end) : Node(NodeType::BINOP, start, end), left(std::move(left)),
                                         right(std::move(right)), op(op) {
}

bool BinopNode::equal(const Node& x) const {
    auto& other = (BinopNode&) x;
    return this->op == other.op && *this->left == *other.left and *this->right == *other.right;
}

BinopNode::~BinopNode() {
    // delete this->left;
    // delete this->right;
}

nlohmann::json BinopNode::to_json() const {
    nlohmann::json j;
    j["type"] = "binop";
    j["binop"]["op"] = op_to_string(this->op);
    j["binop"]["left"] = this->left->to_json();
    j["binop"]["right"] = this->right->to_json();
    return j;
}


