//
// Created by chris on 2/8/20.
//

#include "ScopeError.h"

bool ScopeError::operator==(const ScopeError& other) const {
    std::string a = this->what();
    std::string b = other.what();
    bool t = a == b;
    return t;
}

ScopeError::ScopeError(std::string name) : runtime_error("Name '" + name + "' not found in current scope") {
}
