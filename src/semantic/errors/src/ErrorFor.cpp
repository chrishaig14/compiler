//
// Created by chris on 12/8/21.
//

#include "../include/ErrorFor.h"

Error* ErrorFor::clone() const {
    return nullptr;
}

bool ErrorFor::equal(const Error& other) const {
    auto& o = static_cast<const ErrorFor&>(other);
    return this->entity == o.entity and this->position == o.position;
}

std::string ErrorFor::to_str(const CodeLines& code) const {
    return "Error For";
}

ErrorFor::ErrorFor(const Entity& entity, TextPosition position) : entity(entity), position(position) {

}
