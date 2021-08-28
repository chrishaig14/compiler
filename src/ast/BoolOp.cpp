//
// Created by chris on 25/11/20.
//

#include "BoolOp.h"
using namespace ast;

BoolOp::BoolOp(BoolOpType op, UNode& left, UNode& right, TextPosition start,
                       TextPosition end) : ast::Node(NodeType::BOOLOP, start, end), left(std::move(left)),
                                           right(std::move(right)), op(op) {
}

bool BoolOp::equal(const ast::Node& x) const {
    auto& other = (BoolOp&) x;
    return this->op == other.op && *this->left == *other.left and *this->right == *other.right;
}

BoolOp::~BoolOp() {
    // delete this->left;
    // delete this->right;
}

nlohmann::json BoolOp::to_json() const {
    return {{"type",  "boolop"},
            {"left",  this->left->to_json()},
            {"right", this->right->to_json()},
            {"op",    bool_op_to_string(this->op)}};
}
