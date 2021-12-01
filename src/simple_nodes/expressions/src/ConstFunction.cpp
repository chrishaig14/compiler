//
// Created by chris on 31/8/21.
//

#include "../include/ConstFunction.h"

bool sem::ConstFunction::equals(const sem::Exp& o) const {
    auto& other = (const ConstFunction&) o;
    bool path_ok = this->path.as_str() == other.path.as_str();
    return path_ok;
}

sem::ConstFunction::ConstFunction(Path path) : Exp(ExpType::CONST_FUNCTION), path(path) {
}

sem::UExp sem::ConstFunction::clone() const {
    return std::make_unique<ConstFunction>(*this);
}
