//
// Created by chris on 4/4/21.
//

#include "../include/CallExp.h"
#include "../../common/include/Call.h"

using namespace sem;

CallExp::CallExp(const sem::Exp& function, std::vector<UExp> arguments,
                 std::vector<std::unique_ptr<InstanceObject>> instances)
        : Exp(ExpType::CALL), function(function.clone()), arguments(std::move(arguments)),
          instances(std::move(instances)) {
}

bool CallExp::equals(const Exp& o) const {
    auto& other = (const CallExp&) o;
    return *this->function == *other.function && this->arguments == other.arguments;
}

std::unique_ptr<Call> CallExp::to_call() {
    return std::make_unique<Call>(std::move(this->function), std::move(this->arguments));
}

UExp CallExp::clone() const {
    return std::make_unique<CallExp>(*this);
}
