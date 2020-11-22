//
// Created by chris on 2/8/20.
//

#include "RedeclareError.h"

RedeclareError::RedeclareError(std::string name) : runtime_error("Name \"" + name + "\" already declared in current scope") {
}

bool RedeclareError::operator==(const RedeclareError& other) const {
    std::string a = this->what();
    std::string b = other.what();
    return a == b;
}
