//
// Created by chris on 9/5/21.
//

#include "../include/UnaryOp.h"

using namespace ast;

bool UnaryOp::equal(const ast::ExpNode& other) const {
    return false;
}

UnaryOp::UnaryOp(UnaryOpType op, ast::UExpNode exp, TextPosition start, TextPosition end) : ast::ExpNode(ExpNodeType::UNARY,
                                                                                              start,
                                                                                              end),
                                                                                    exp(std::move(exp)) {
    this->op = op;
}

nlohmann::json UnaryOp::to_json() const {
    return nlohmann::json();
}
