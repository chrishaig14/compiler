//
// Created by chris on 1/8/20.
//

#include "TypeNode.h"

FunctionTypeNode::FunctionTypeNode(const VectorOfTypes& parameterTypes,
                                   TypeNode& returnType) {
    for (auto p: parameterTypes) {
        this->parameter_types.emplace_back(p->clone());
    }
    this->return_type = returnType.clone();
    this->kind = Kind::FUNCTION;
}

FunctionTypeNode* FunctionTypeNode::clone() const {
    VectorOfTypes aux;
    for (auto p: this->parameter_types) {
        aux.emplace_back(p->clone());
    }
    return new FunctionTypeNode(aux, *this->return_type);
}

FunctionTypeNode::~FunctionTypeNode() {
    delete this->return_type;
    for (auto p: this->parameter_types) {
        delete p;
    }
}

std::string FunctionTypeNode::to_string() const {
    auto& ftype = *this;
    std::string parameters;
    std::string ret;
    for (auto ptr: ftype.parameter_types) {
        auto& p = *ptr;
        parameters += p.to_string() + ", ";
    }
    if (ftype.parameter_types.size() != 0) {
        parameters = parameters.substr(0, parameters.size() - 2);
    }
    ret = ftype.return_type->to_string() + ", ";
    return "fun (" + parameters + ") . " + ret;
}

bool FunctionTypeNode::equal(const TypeNode& other) const {
    auto& a = *this;
    auto& b = other.function();
    if (a.parameter_types.size() != b.parameter_types.size()) {
        return false;
    }
    for (int i = 0; i < a.parameter_types.size(); i++) {
        if (*a.parameter_types[i] != *b.parameter_types[i]) {
            return false;
        }
    }
    return *a.return_type == *b.return_type;
}

FunctionTypeNode& FunctionTypeNode::function() { return *this; }

const FunctionTypeNode& FunctionTypeNode::function() const { return *this; }

ObjectTypeNode::ObjectTypeNode(const std::string& identifier,
                               const VectorOfTypes& typeParameters) : identifier(
        identifier), type_parameters(typeParameters) {
    this->kind = Kind::OBJECT;
}

TypeNode* ObjectTypeNode::clone() const {
    VectorOfTypes aux;
    for (auto p: this->type_parameters) {
        aux.emplace_back(p->clone());
    }
    return new ObjectTypeNode(this->identifier, aux);
}

ObjectTypeNode::~ObjectTypeNode() {
    for (auto p: this->type_parameters) {
        delete p;
    }
}

std::string ObjectTypeNode::to_string() const {
    auto& otype = *this;
    std::string parameters;
    for (auto ptr: otype.type_parameters) {
        auto& p = *ptr;
        parameters += p.to_string() + ", ";
    }
    if (parameters.size() != 0) {
        parameters = parameters.substr(0, parameters.size() - 2);
        return otype.identifier + "[" + parameters + "]";
    }
    return otype.identifier;
}

bool ObjectTypeNode::equal(const TypeNode& other) const {
    auto& a = *this;
    auto& b = other.object();
    if (a.identifier != b.identifier) {
        return false;
    }
    if (a.type_parameters.size() != b.type_parameters.size()) {
        return false;
    }
    for (int i = 0; i < a.type_parameters.size(); i++) {
        if (*a.type_parameters[i] != *b.type_parameters[i]) {
            return false;
        }
    }
    return true;
}

const ObjectTypeNode& ObjectTypeNode::object() const { return *this; }

ObjectTypeNode& ObjectTypeNode::object() { return *this; }

bool TypeNode::operator!=(const TypeNode& other) const {
    return !(*this == other);
}

bool TypeNode::operator==(const TypeNode& other) const {
    if (this->kind != other.kind) {
        return false;
    }
    return this->equal(other);
}

ObjectTypeNode& TypeNode::object() { throw std::runtime_error("Getting wrong type!"); }

FunctionTypeNode& TypeNode::function() { throw std::runtime_error("Getting wrong type!"); }

const FunctionTypeNode& TypeNode::function() const { throw std::runtime_error("Getting wrong type!"); }

const ObjectTypeNode& TypeNode::object() const { throw std::runtime_error("Getting wrong type!"); }
