//
// Created by chris on 24/3/21.
//

#include "../Cast.h"

using namespace ast;

Cast::Cast(Statement* exp, std::string as_type, TextPosition start, TextPosition end) : Statement(StatementType::CAST, start, end) {
    this->exp = exp;
    this->as_type = as_type;
}

bool Cast::equal(const ast::Statement& x) const {
    return true;
}

Cast::~Cast() = default;
