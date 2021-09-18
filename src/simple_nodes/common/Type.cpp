//
// Created by chris on 1/8/20.
//

#include <cassert>
#include "Type.h"

using namespace sem;

bool Type::operator!=(const sem::Type& other) const {
    return !(*this == other);
}

bool Type::operator==(const sem::Type& other) const {
    if (this->kind != other.kind) {
        return false;
    }
    return this->equal(other);
}

sem::TypeObject& Type::object() {
    throw std::runtime_error("Getting wrong type!");
}

sem::TypeFunction& Type::function() {
    throw std::runtime_error("Getting wrong type!");
}

const sem::TypeFunction& Type::function() const {
    throw std::runtime_error("Getting wrong type!");
}

const sem::TypeObject& Type::object() const {
    throw std::runtime_error("Getting wrong type!");
}

bool Type::is_generic() const {
    return false;
}

std::ostream& operator<<(std::ostream& o, const sem::Type& t) {
    o << t.to_string();
    return o;
}
