//
// Created by chris on 12/8/21.
//

#include "../include/For.h"

using namespace error;

Error* For::clone() const {
    return nullptr;
}

bool For::equal(const Error& other) const {
    auto& o = static_cast<const For&>(other);
    bool e_ok = *this->entity == *o.entity;
    return e_ok and this->position == o.position;
}

std::string For::to_str(const CodeLines& code) const {
    return "Error For";
}

For::For(const Entity& entity, TextPosition position) : Error(ErrorType::for_error),entity(entity.clone()), position(position) {

}
