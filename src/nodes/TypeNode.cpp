//
// Created by chris on 1/8/20.
//

#include "TypeNode.h"

FunctionType::FunctionType(const VectorOfTypes& parameterTypes,
                           TypeNode* returnType) {

    for (auto p: parameterTypes) {
        assert(p != nullptr);
    }
    assert(returnType != nullptr);
    this->param_types = parameterTypes;
    this->return_type = returnType;
    this->kind = Kind::FUNCTION;
}

FunctionType* FunctionType::clone() const {
    VectorOfTypes aux;
    for (auto p: this->param_types) {
        aux.emplace_back(p->clone());
    }
    return new FunctionType(aux, this->return_type->clone());
}

FunctionType::~FunctionType() {
    delete this->return_type;
    for (auto p: this->param_types) {
        delete p;
    }
}

std::string FunctionType::to_string() const {
    auto& ftype = *this;
    std::string parameters;
    std::string ret;
    for (auto ptr: ftype.param_types) {
        auto& p = *ptr;
        parameters += p.to_string() + ", ";
    }
    if (ftype.param_types.size() != 0) {
        parameters = parameters.substr(0, parameters.size() - 2);
    }
    ret = ftype.return_type->to_string();
    return "fun (" + parameters + ")" + (*ftype.return_type == ObjectType(".None") ? "" : (" -> " + ret));
}

bool FunctionType::equal(const TypeNode& other) const {
    auto& a = *this;
    auto& b = other.function();
    if (a.param_types.size() != b.param_types.size()) {
        return false;
    }
    for (size_t i = 0; i < a.param_types.size(); i++) {
        if (*a.param_types[i] != *b.param_types[i]) {
            return false;
        }
    }
    return *a.return_type == *b.return_type;
}

FunctionType& FunctionType::function() { return *this; }

const FunctionType& FunctionType::function() const { return *this; }

ObjectType::ObjectType(const std::string& identifier,
                       const VectorOfTypes& typeParameters) : id(
        identifier
), type_params(typeParameters) {
    for (auto p: typeParameters) {
        assert(p != nullptr);
    }
    this->kind = Kind::OBJECT;
}

TypeNode* ObjectType::clone() const {
    VectorOfTypes aux;
    for (auto p: this->type_params) {
        aux.emplace_back(p->clone());
    }
    return new ObjectType(this->id, aux);
}

ObjectType::~ObjectType() {
    for (auto p: this->type_params) {
        delete p;
    }
}

std::string ObjectType::to_string() const {
    auto& otype = *this;
    std::string parameters;
    for (auto ptr: otype.type_params) {
        auto& p = *ptr;
        parameters += p.to_string() + ", ";
    }
    if (parameters.size() != 0) {
        parameters = parameters.substr(0, parameters.size() - 2);
        return otype.id + "[" + parameters + "]";
    }
    return otype.id;
}

bool ObjectType::equal(const TypeNode& other) const {
    auto& a = *this;
    auto& b = other.object();
    if (a.id != b.id) {
        return false;
    }
    if (a.type_params.size() != b.type_params.size()) {
        return false;
    }
    for (size_t i = 0; i < a.type_params.size(); i++) {
        if (*a.type_params[i] != *b.type_params[i]) {
            return false;
        }
    }
    return true;
}

const ObjectType& ObjectType::object() const { return *this; }

ObjectType& ObjectType::object() { return *this; }

ObjectType::ObjectType(const std::string& identifier) : ObjectType(identifier, {}) {
}

bool TypeNode::operator!=(const TypeNode& other) const {
    return !(*this == other);
}

bool TypeNode::operator==(const TypeNode& other) const {
    if (this->kind != other.kind) {
        return false;
    }
    return this->equal(other);
}

ObjectType& TypeNode::object() { throw std::runtime_error("Getting wrong type!"); }

FunctionType& TypeNode::function() { throw std::runtime_error("Getting wrong type!"); }

const FunctionType& TypeNode::function() const { throw std::runtime_error("Getting wrong type!"); }

const ObjectType& TypeNode::object() const { throw std::runtime_error("Getting wrong type!"); }
