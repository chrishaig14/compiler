//
// Created by chris on 9/5/21.
//

#include "UnaryOp.h"
using namespace ast;
bool UnaryOp::equal(const ast::Node& other) const {
    return false;
}

UnaryOp::UnaryOp(UnaryOpType op, ast::Node* exp, TextPosition start, TextPosition end) : ast::Node(NodeType::UNARY, start, end) {
    this->op = op;
    this->exp = exp;
}

nlohmann::json UnaryOp::to_json() const {
    return nlohmann::json();
}
