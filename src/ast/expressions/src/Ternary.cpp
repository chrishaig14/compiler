//
// Created by chris on 29/8/20.
//

#include "../include/Ternary.h"
#include <util/utils.h>

using namespace ast;

bool Ternary::equal(const ast::ExpNode& x) const {
    auto& other = (Ternary&) x;
    return *this->expression == *other.expression && *this->true_case == *other.true_case &&
           *this->false_case == *other.false_case;
}

Ternary::Ternary(ExpNode* expression, ast::ExpNode* trueCase, ast::ExpNode* falseCase, TextPosition start, TextPosition end)
        : ast::ExpNode(ExpNodeType::TERNARY, start, end), expression(expression), true_case(trueCase), false_case(falseCase) {
}

Ternary::~Ternary() {
    delete expression;
    delete true_case;
    delete false_case;
}
