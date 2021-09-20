//
// Created by chris on 24/3/21.
//

#include "Cast.h"

using namespace ast;

Cast::Cast(CommonNode* exp, std::string as_type, TextPosition start, TextPosition end) : CommonNode(CommonNodeType::CAST, start, end) {
    this->exp = exp;
    this->as_type = as_type;
}

bool Cast::equal(const ast::CommonNode& x) const {
    return true;
}

Cast::~Cast() = default;
