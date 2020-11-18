//
// Created by chris on 1/8/20.
//

#include "TypeNode.h"

FunctionTypeNode::FunctionTypeNode(const VectorOfTypes& parameterTypes,
                                   TypeNode* returnType)
        : parameter_types(parameterTypes), return_type(returnType) {}

TypeNode* FunctionTypeNode::clone() const {
    VectorOfTypes aux;
    for (auto p: this->parameter_types) {
        aux.emplace_back(p->clone());
    }
    return new FunctionTypeNode(aux, this->return_type->clone());
}

ObjectTypeNode::ObjectTypeNode(const std::string& identifier,
                               const VectorOfTypes& typeParameters) : identifier(
        identifier), type_parameters(typeParameters) {

}

TypeNode* ObjectTypeNode::clone() const {
    VectorOfTypes aux;
    for (auto p: this->type_parameters) {
        aux.emplace_back(p->clone());
    }
    return new ObjectTypeNode(this->identifier, aux);
}
