//
// Created by chris on 4/4/21.
//

#include "../include/CallExp.h"
#include "../../common/include/Call.h"

using namespace sem;

CallExp::CallExp(UExp function, std::vector<UExp> arguments)
        : Exp(ExpType::CALL), function(std::move(function)), arguments(std::move(arguments)) {
}

bool CallExp::equals(const Exp& o) const {
    auto& other = (const CallExp&) o;
    return *this->function == *other.function && this->arguments == other.arguments;
}

std::unique_ptr<Call> CallExp::to_call() {
    return std::make_unique<Call>(std::move(this->function), std::move(this->arguments));
}
