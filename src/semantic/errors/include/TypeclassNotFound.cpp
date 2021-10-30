//
// Created by chris on 25/10/21.
//

#include "TypeclassNotFound.h"

using namespace error;

error::TypeclassNotFound::TypeclassNotFound(const std::string& name)
        : Error(ErrorType::typeclass_not_found), name(name) {
}

bool error::TypeclassNotFound::equal(const error::Error& other) const {
    return false;
}

std::string error::TypeclassNotFound::to_str(const CodeLines& code) const {
    return "typeclass not found";
}

error::Error* error::TypeclassNotFound::clone() const {
    return new TypeclassNotFound(this->name);
}
