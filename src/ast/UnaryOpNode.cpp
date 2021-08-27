//
// Created by chris on 9/5/21.
//

#include "UnaryOpNode.h"
using namespace ast;
bool UnaryOpNode::equal(const ast::Node& other) const {
    return false;
}

UnaryOpNode::UnaryOpNode(UnaryOp op, ast::Node* exp, TextPosition start, TextPosition end) : ast::Node(NodeType::UNARY, start, end) {
    this->op = op;
    this->exp = exp;
}

nlohmann::json UnaryOpNode::to_json() const {
    return nlohmann::json();
}
