//
// Created by chris on 1/8/20.
//

#include "BinaryOp.h"

using namespace ast;

BinaryOp::BinaryOp(OpType op, UNode left, UNode right, TextPosition start, TextPosition end)
        : ast::Node(NodeType::BINOP, start, end), _left(std::move(left)), _right(std::move(right)), left(*_left),
          right(*_right), op(op) {
}

bool BinaryOp::equal(const ast::Node& x) const {
    auto& other = (BinaryOp&) x;
    return this->op == other.op && this->left == other.left and this->right == other.right;
}

BinaryOp::~BinaryOp() {
    // delete this->left;
    // delete this->right;
}

nlohmann::json BinaryOp::to_json() const {
    nlohmann::json j;
    j["type"] = "binop";
    j["binop"]["op"] = op_to_string(this->op);
    j["binop"]["left"] = this->left.to_json();
    j["binop"]["right"] = this->right.to_json();
    return j;
}


