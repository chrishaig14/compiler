//
// Created by chris on 24/10/21.
//

#include "MyErrorFormatter.h"

std::string MyErrorFormatter::format(const error::BadReturn& err) const {
    return "Error::BadReturn";
}

std::string MyErrorFormatter::format(const error::TypeMismatch& err) const {
    return "Error::TypeMismatch: expected " + err.expected->to_string() + " but got " + entity_to_string(*err.actual);
}

std::string MyErrorFormatter::format(const error::TypeclassNotFound& err) const {
    return "Error::TypeclassNotFound: " + err.name;
}
