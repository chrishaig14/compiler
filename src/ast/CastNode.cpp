//
// Created by chris on 24/3/21.
//

#include "CastNode.h"
using namespace ast;
CastNode::CastNode(Node* exp, std::string as_type, TextPosition start, TextPosition end):Node(NodeType::CAST, start, end) {
    this->exp = exp;
    this->as_type = as_type;
}

bool CastNode::equal(const ast::Node& x) const {
    return true;
}

CastNode::~CastNode() = default;
