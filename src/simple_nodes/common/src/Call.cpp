//
// Created by chris on 4/4/21.
//

#include "../include/Call.h"
#include "../../expressions/include/Exp.h"

using namespace sem;

Call::Call(UExp function, std::vector<UExp> arguments)
        : Common(CommonType::CALL), function(std::move(function)), arguments(std::move(arguments)) {
}

bool Call::equals(const Common& o) const {
    auto& other = (const Call&) o;
    return *this->function == *other.function && this->arguments == other.arguments;
}
