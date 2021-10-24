//
// Created by chris on 12/8/21.
//

#include "../include/ErrorUnusedReturnValue.h"
#include <ast/statements/Call.h>

Error* ErrorUnusedReturnValue::clone() const {
    return nullptr;
}

bool ErrorUnusedReturnValue::equal(const Error& other) const {
    auto& o = static_cast<const ErrorUnusedReturnValue&>(other);
    return *this->entity == *o.entity and this->node == o.node;
}

std::string ErrorUnusedReturnValue::to_str(const CodeLines& code) const {
    return "Error: unused return value " + entity_to_string(*this->entity);
}

ErrorUnusedReturnValue::ErrorUnusedReturnValue(const Entity& entity, const ast::Call& node) : node(node), entity(entity.clone()) {
}
