//
// Created by chris on 1/8/20.
//

#include "../include/BinaryOp.h"

using namespace ast;

BinaryOp::BinaryOp(OpType op, ast::UExpNode left, ast::UExpNode right, TextPosition start, TextPosition end)
        : ast::ExpNode(ExpNodeType::BINOP, start, end), _left(std::move(left)), _right(std::move(right)), left(*_left),
          right(*_right), op(op) {
}

bool BinaryOp::equal(const ast::ExpNode& x) const {
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


