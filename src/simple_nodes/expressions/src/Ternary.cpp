//
// Created by chris on 8/5/21.
//

#include "../include/Ternary.h"
#include <iostream>

using namespace sem;

Ternary::Ternary(UExp exp, UExp true_case, UExp false_case) : Exp(ExpType::TERNARY) {
    this->exp = std::move(exp);
    this->true_case = std::move(true_case);
    this->false_case = std::move(false_case);
}

bool Ternary::equals(const Exp& o) const {
    // auto& other = (const Ternary&) o;
    return false;
}

UExp Ternary::clone() const {
    return std::make_unique<Ternary>(*this);
}

Ternary::Ternary(const Ternary& other) : sem::Exp(ExpType::TERNARY) {
    this->exp = other.exp->clone();
    this->true_case = other.true_case->clone();
    this->false_case = other.false_case->clone();
}
