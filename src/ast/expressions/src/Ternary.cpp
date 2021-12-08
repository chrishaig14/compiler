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

nlohmann::json Ternary::to_json() const {
    return nlohmann::json();
}

Ternary::Ternary(ast::UExpNode expression, ast::UExpNode trueCase, ast::UExpNode falseCase, TextPosition start,
                 TextPosition end) : ast::ExpNode(ExpNodeType::TERNARY, start, end) {
    this->expression = std::move(expression);
    this->true_case = std::move(trueCase);
    this->false_case = std::move(falseCase);
}
