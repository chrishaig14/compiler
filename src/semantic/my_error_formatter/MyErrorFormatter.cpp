//
// Created by chris on 24/10/21.
//

#include "MyErrorFormatter.h"

std::string MyErrorFormatter::format(const error::BadReturn& err) const {
    return "Error::BadReturn";
}

std::string MyErrorFormatter::format(const error::TypeMismatch& err) const {
    return "Error::TypeMismatch";
}
