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
    if (*this->function != *other.function) {
        return false;
    }
    if (this->arguments.size() != other.arguments.size()) {
        return false;
    }
    for (size_t i = 0; i < this->arguments.size(); i++) {
        if (*this->arguments[i] != *other.arguments[i]) {
            return false;
        }
    }
    return true;
}
