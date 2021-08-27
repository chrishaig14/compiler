//
// Created by chris on 25/11/20.
//

#include "BoolOpNode.h"
using namespace ast;

BoolOpNode::BoolOpNode(BoolOp op, UNode& left, UNode& right, TextPosition start,
                       TextPosition end) : ast::Node(NodeType::BOOLOP, start, end), left(std::move(left)),
                                           right(std::move(right)), op(op) {
}

bool BoolOpNode::equal(const ast::Node& x) const {
    auto& other = (BoolOpNode&) x;
    return this->op == other.op && *this->left == *other.left and *this->right == *other.right;
}

BoolOpNode::~BoolOpNode() {
    // delete this->left;
    // delete this->right;
}

nlohmann::json BoolOpNode::to_json() const {
    return {{"type",  "boolop"},
            {"left",  this->left->to_json()},
            {"right", this->right->to_json()},
            {"op",    bool_op_to_string(this->op)}};
}
