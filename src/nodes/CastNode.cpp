//
// Created by chris on 24/3/21.
//

#include "CastNode.h"

CastNode& CastNode::cast() {
    return *this;
}

const CastNode& CastNode::cast() const {
    return *this;
}

CastNode::CastNode(Node* exp, std::string as_type, TextPosition start, TextPosition end):Node(NodeType::CAST, start, end) {
    this->exp = exp;
    this->as_type = as_type;
}

bool CastNode::equal(const Node& x) const {
    return true;
}

CastNode::~CastNode() = default;
