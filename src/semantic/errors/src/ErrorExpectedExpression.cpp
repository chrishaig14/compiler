//
// Created by chris on 12/8/21.
//

#include "../include/ErrorExpectedExpression.h"

Error* ErrorExpectedExpression::clone() const {
    return new ErrorExpectedExpression(*this->entity, this->node);
}

bool ErrorExpectedExpression::equal(const Error& other) const {
    auto& o = (const ErrorExpectedExpression&) other;
    bool entity_ok = *this->entity == *o.entity;
    bool node_ok = this->node == o.node;
    return entity_ok and node_ok;
}

std::string ErrorExpectedExpression::to_str(const CodeLines& code) const {
    return "Error: expected expression, got " + entity_to_string(*this->entity);
}

ErrorExpectedExpression::ErrorExpectedExpression(const Entity& entity, const ast::ExpNode& node)
        : entity(entity.clone()), node(node) {

}
