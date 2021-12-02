//
// Created by chris on 19/6/21.
//

#include "../include/Throw.h"
using namespace sem;
Throw::Throw(Common* expression) : Common(CommonType::THROW), expression(expression) {
}

bool Throw::equals(const Common& o) const {
    return false;
}

std::unique_ptr<Common> Throw::clone() const {
    return std::unique_ptr<Common>();
}
