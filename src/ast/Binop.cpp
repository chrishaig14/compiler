//
// Created by chris on 1/8/20.
//

#include "Binop.h"

using namespace ast;

Binop::Binop(OpType op, UNode& left, UNode& right, TextPosition start, TextPosition end) : ast::Node(NodeType::BINOP,
                                                                                                        start,
                                                                                                        end),
                                                                                                   left(std::move(left)),
                                                                                                   right(std::move(right)),
                                                                                                   op(op) {
}

bool Binop::equal(const ast::Node& x) const {
    auto& other = (Binop&) x;
    return this->op == other.op && *this->left == *other.left and *this->right == *other.right;
}

Binop::~Binop() {
    // delete this->left;
    // delete this->right;
}

nlohmann::json Binop::to_json() const {
    nlohmann::json j;
    j["type"] = "binop";
    j["binop"]["op"] = op_to_string(this->op);
    j["binop"]["left"] = this->left->to_json();
    j["binop"]["right"] = this->right->to_json();
    return j;
}


