//
// Created by chris on 12/8/21.
//

#include "../include/UnusedReturnValue.h"
#include <ast/statements/Call.h>

using namespace error;

Error* UnusedReturnValue::clone() const {
    return nullptr;
}

bool UnusedReturnValue::equal(const Error& other) const {
    auto& o = static_cast<const UnusedReturnValue&>(other);
    return *this->entity == *o.entity and this->node == o.node;
}

std::string UnusedReturnValue::to_str(const CodeLines& code) const {
    return "Error: unused return value " + entity_to_string(*this->entity);
}

UnusedReturnValue::UnusedReturnValue(const Entity& entity, const ast::Call& node)
        :Error(ErrorType::unused_return_value), node(node), entity(entity.clone()) {
}
