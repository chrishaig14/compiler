//
// Created by chris on 24/3/21.
//

#include "Cast.h"

using namespace ast;

Cast::Cast(Node* exp, std::string as_type, TextPosition start, TextPosition end) : Node(NodeType::CAST, start, end) {
    this->exp = exp;
    this->as_type = as_type;
}

bool Cast::equal(const ast::Node& x) const {
    return true;
}

Cast::~Cast() = default;
