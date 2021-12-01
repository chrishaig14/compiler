//
// Created by chris on 4/4/21.
//

#include "../include/Id.h"

using namespace sem;

Id::Id(const std::string& identifier) : Exp(ExpType::ID) {
    this->identifier = identifier;
}

bool Id::equals(const Exp& o) const {
    auto& other = (const Id&) o;
    return this->identifier == other.identifier;
}

UExp Id::clone() const {
    return std::make_unique<Id>(*this);
}
