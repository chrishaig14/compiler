//
// Created by chris on 29/8/20.
//

#include "Ternary.h"
#include "../utils.h"
using namespace ast;
bool Ternary::equal(const ast::Node& x) const {
    auto& other = (Ternary&)x;
    return *this->expression == *other.expression && *this->true_case == *other.true_case &&
           *this->false_case == *other.false_case;
}

Ternary::Ternary(Node* expression, ast::Node* trueCase, ast::Node* falseCase, TextPosition start, TextPosition end) : ast::Node(NodeType::TERNARY, start, end ), expression(
        expression),
                                                                              true_case(
                                                                                      trueCase),
                                                                              false_case(
                                                                                      falseCase) {  }

Ternary::~Ternary() {
    delete expression;
    delete true_case;
    delete false_case;
}
