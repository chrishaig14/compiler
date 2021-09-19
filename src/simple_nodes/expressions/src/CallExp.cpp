//
// Created by chris on 4/4/21.
//

#include "../include/CallExp.h"

using namespace sem;

CallExp::CallExp(UExp function, std::vector<UExp> arguments)
        : Exp(ExpType::CALL), function(std::move(function)), arguments(std::move(arguments)) {
}

bool CallExp::equals(const Exp& o) const {
    auto& other = (const CallExp&) o;
    return *this->function == *other.function && this->arguments == other.arguments;
}
