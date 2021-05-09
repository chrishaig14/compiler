//
// Created by chris on 9/5/21.
//

#include "UnaryOpNode.h"

bool UnaryOpNode::equal(const Node& other) const {
    return false;
}

UnaryOpNode::UnaryOpNode(UnaryOp op, Node* exp, TextPosition start, TextPosition end) : Node(NodeType::UNARY, start, end) {
    this->op = op;
    this->exp = exp;
}
