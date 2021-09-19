//
// Created by chris on 8/5/21.
//

#include "../include/Ternary.h"

using namespace sem;

Ternary::Ternary(Exp* ext, Exp* true_case, Exp* false_case) : Exp(ExpType::TERNARY) {
    this->ext = ext;
    this->true_case = true_case;
    this->false_case = false_case;
}

bool Ternary::equals(const Exp& o) const {
    // auto& other = (const Ternary&) o;
    return false;
}
