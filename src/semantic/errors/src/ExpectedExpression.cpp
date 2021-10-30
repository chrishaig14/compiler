//
// Created by chris on 12/8/21.
//

#include "../include/ExpectedExpression.h"

using namespace error;

Error* ExpectedExpression::clone() const {
    return new ExpectedExpression(*this->entity, this->node);
}

bool ExpectedExpression::equal(const Error& other) const {
    auto& o = (const ExpectedExpression&) other;
    bool entity_ok = *this->entity == *o.entity;
    bool node_ok = this->node == o.node;
    return entity_ok and node_ok;
}

std::string ExpectedExpression::to_str(const CodeLines& code) const {
    return "Error: expected expression, got " + entity_to_string(*this->entity);
}

ExpectedExpression::ExpectedExpression(const Entity& entity, const ast::ExpNode& node)
        :Error(ErrorType::expected_expression), entity(entity.clone()), node(node) {

}
