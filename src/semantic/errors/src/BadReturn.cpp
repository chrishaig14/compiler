//
// Created by chris on 12/8/21.
//

#include "../include/BadReturn.h"

using namespace error;

Error* BadReturn::clone() const {
    return nullptr;
}

bool BadReturn::equal(const Error& other) const {
    return false;
}

std::string BadReturn::to_str(const CodeLines& code) const {
    return std::string();
}

BadReturn::BadReturn(TextPosition position) {

}
