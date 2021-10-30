//
// Created by chris on 30/10/21.
//

#include "GenericError.h"

using namespace error;

Error* GenericError::clone() const {
    return new GenericError(this->msg, this->start);
}

bool GenericError::equal(const Error& other) const {
    return static_cast<const GenericError&>(other).msg == this->msg;
}

std::string GenericError::to_str(const CodeLines& code) const {
    return std::string();
}
