//
// Created by chris on 1/8/20.
//

#include <cassert>
#include "TypeNode.h"

FunctionType::FunctionType(const VectorOfTypes& parameterTypes, TypeNode* returnType) {

    for (auto *p: parameterTypes) {
        assert(p != nullptr);
    }
    assert(returnType != nullptr);
    this->param_types = parameterTypes;
    this->return_type = returnType;
    this->kind = Kind::FUNCTION;
}

FunctionType* FunctionType::clone() const {
    VectorOfTypes aux;
    for (auto *p: this->param_types) {
        aux.emplace_back(p->clone());
    }
    return new FunctionType(aux, this->return_type->clone());
}

FunctionType::~FunctionType() {
    delete this->return_type;
    for (auto *p: this->param_types) {
        delete p;
    }
}

std::string FunctionType::to_string() const {
    const auto& ftype = *this;
    std::string parameters;
    std::string ret;
    for (auto *ptr: ftype.param_types) {
        auto& p = *ptr;
        parameters += p.to_string() + ", ";
    }
    if (!ftype.param_types.empty()) {
        parameters = parameters.substr(0, parameters.size() - 2);
    }
    ret = ftype.return_type->to_string();
    return "fun (" + parameters + ")" + (*ftype.return_type == ObjectType(".None") ? "" : (" -> " + ret));
}

std::string FunctionType::actual_to_string() const {
    const auto& ftype = *this;
    std::string parameters;
    std::string ret;
    for (auto *ptr: ftype.param_types) {
        auto& p = *ptr;
        parameters += p.actual_to_string() + ", ";
    }
    if (!ftype.param_types.empty()) {
        parameters = parameters.substr(0, parameters.size() - 2);
    }
    ret = ftype.return_type->actual_to_string();
    return "fun (" + parameters + ")" + (*ftype.return_type == ObjectType(".None") ? "" : (" -> " + ret));
}

bool FunctionType::equal(const TypeNode& other) const {
    const auto& a = *this;
    const auto& b = other.function();
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

FunctionType& FunctionType::function() {
    return *this;
}

const FunctionType& FunctionType::function() const {
    return *this;
}

bool FunctionType::is_generic() const {
    for (auto *t: this->param_types) {
        if (t->is_generic()) {
            return true;
        }
    }
    return false;
}

ObjectType::ObjectType(const std::string& identifier, const VectorOfTypes& typeParameters)
        : id(identifier), type_params(typeParameters) {
    for (auto *p: typeParameters) {
        assert(p != nullptr);
    }
    this->kind = Kind::OBJECT;
    this->aliased_type = nullptr;
    this->is_generic_param = false;
}

TypeNode* ObjectType::clone() const {
    VectorOfTypes aux;
    for (auto *p: this->type_params) {
        aux.emplace_back(p->clone());
    }

    auto *n = new ObjectType(this->id, aux);
    n->actual_base_path = this->actual_base_path;
    n->is_generic_param = this->is_generic_param;
    n->aliased_type = this->aliased_type;
    return n;
}

ObjectType::~ObjectType() {
    for (auto *p: this->type_params) {
        delete p;
    }
}

std::string ObjectType::to_string() const {
    const auto& otype = *this;
    std::string parameters;
    for (auto *ptr: otype.type_params) {
        auto& p = *ptr;
        parameters += p.to_string() + ", ";
    }
    if (!parameters.empty()) {
        parameters = parameters.substr(0, parameters.size() - 2);
        return otype.id + "[" + parameters + "]";
    }
    return otype.id;
}

std::string ObjectType::actual_to_string() const {
    const auto& otype = *this;
    std::string parameters;
    for (auto *ptr: otype.type_params) {
        auto& p = *ptr;
        parameters += p.actual_to_string() + ", ";
    }
    if (!parameters.empty()) {
        parameters = parameters.substr(0, parameters.size() - 2);
        return this->actual_base_path.as_str() + "[" + parameters + "]";
    }
    return this->actual_base_path.as_str();
}

bool ObjectType::equal(const TypeNode& other) const {
    const auto& a = *this;
    const auto& b = other.object();
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

const ObjectType& ObjectType::object() const {
    return *this;
}

ObjectType& ObjectType::object() {
    return *this;
}

ObjectType::ObjectType(const std::string& identifier) : ObjectType(identifier, {}) {
    this->is_generic_param = false;
}

bool ObjectType::is_generic() const {
    if (this->is_generic_param) {
        return true;
    }
    for (auto *t: this->type_params) {
        if (t->is_generic()) {
            return true;
        }
    }
    return false;
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

ObjectType& TypeNode::object() {
    throw std::runtime_error("Getting wrong type!");
}

FunctionType& TypeNode::function() {
    throw std::runtime_error("Getting wrong type!");
}

const FunctionType& TypeNode::function() const {
    throw std::runtime_error("Getting wrong type!");
}

const ObjectType& TypeNode::object() const {
    throw std::runtime_error("Getting wrong type!");
}

bool TypeNode::is_generic() const {
    return false;
}
