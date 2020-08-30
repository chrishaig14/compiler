//
// Created by chris on 1/8/20.
//

#include "TypeNode.h"

void FunctionTypeNode::accept(Visitor& visitor) {

}

bool FunctionTypeNode::equal(Node* other) const {
    FunctionTypeNode* other_ptr = dynamic_cast<FunctionTypeNode*>(other);
    if (other_ptr == nullptr) return false;
    return *this == *other_ptr;
}

FunctionTypeNode::FunctionTypeNode(const std::vector<TypeNode*>& parameterTypes, TypeNode* returnType)
        : parameter_types(parameterTypes), return_type(returnType) {}

std::string FunctionTypeNode::to_string() {
    std::string parameters;
    std::string ret;
    for (auto p: this->parameter_types) {
        parameters += p->to_string() + ", ";
    }
    ret = this->return_type->to_string();
    return "fun (" + parameters + ") -> " + ret;
}

void GeneratorTypeNode::accept(Visitor& visitor) {

}

bool GeneratorTypeNode::equal(Node* other) const {
    return false;
}

GeneratorTypeNode::GeneratorTypeNode(const std::vector<TypeNode*>& parameterTypes, TypeNode* returnType)
        : parameter_types(parameterTypes), return_type(returnType) {}

std::string GeneratorTypeNode::to_string() {
    std::string parameters;
    std::string ret;
    for (auto p: this->parameter_types) {
        parameters += p->to_string() + ", ";
    }
    ret = this->return_type->to_string();
    return "gen (" + parameters + ") -> " + ret;
}

void ObjectTypeNode::accept(Visitor& visitor) {

}

bool ObjectTypeNode::equal(Node* other) const {
    const ObjectTypeNode* other_ptr = dynamic_cast<const ObjectTypeNode*>(other);
    if (other_ptr == nullptr) return false;
    return *this == *other_ptr;
}

ObjectTypeNode::ObjectTypeNode(const std::string& identifier, const std::vector<TypeNode*>& typeParameters)
        : identifier(identifier), type_parameters(typeParameters) {}

std::string ObjectTypeNode::to_string() {
    std::string parameters;
    for (auto p: this->type_parameters) {
        parameters += p->to_string() + ", ";
    }
    if (parameters.size() != 0) {
        parameters = parameters.substr(0, parameters.size() - 2);
        return this->identifier + "[" + parameters + "]";
    }
    return this->identifier;
}

