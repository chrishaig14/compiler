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

CastNode::CastNode(Node* exp, std::string as_type, TextPosition start) {
    this->exp = exp;
    this->ntype = CAST;
    this->as_type = as_type;
    this->start = start;
}

bool CastNode::equal(const Node& x) const {
    return true;
}

CastNode::~CastNode() {
}
