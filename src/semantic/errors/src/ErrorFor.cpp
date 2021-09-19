//
// Created by chris on 12/8/21.
//

#include "../include/ErrorFor.h"

Error* ErrorFor::clone() const {
    return nullptr;
}

bool ErrorFor::equal(const Error& other) const {
    return false;
}

std::string ErrorFor::to_str(const CodeLines& code) const {
    return std::string();
}

ErrorFor::ErrorFor(const Entity& entity, TextPosition position) {

}
