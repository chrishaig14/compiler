//
// Created by chris on 1/8/20.
//

#include "DeclarationNode.h"

void DeclarationNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

DeclarationNode::DeclarationNode(const std::string& identifier, TypeNode* type, Node* expression) : identifier(
        identifier),
                                                                                                            type(type),
                                                                                                            expression(
                                                                                                                    expression) {}

bool DeclarationNode::operator==(const DeclarationNode& other) const {
    return this->identifier == other.identifier && this->expression == other.expression &&
           ((this->type != nullptr && other.type !=
                                      nullptr && *this->type == *other.type) || (this->type ==
                                                                                 nullptr && other.type == nullptr));
    return false;
//        if (this->type == nullptr && other.type != nullptr) return false;
//        if (this->type != nullptr && other.type == nullptr) return false;
//        if (this->expression == nullptr && other.expression != nullptr) return false;
//        if (this->expression != nullptr && other.expression == nullptr) return false;
//        bool same_type = this->type == nullptr or this->type->equal(other.type);
//        bool same_expression = this->expression == nullptr or this->expression->equal(other.expression);
//        bool same_identifier = this->identifier == other.identifier;
//        return same_identifier && same_type && same_expression;
}

bool DeclarationNode::operator!=(const DeclarationNode& other) const { return !(*this == other); }

