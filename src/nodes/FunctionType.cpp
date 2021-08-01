//
// Created by chris on 3/7/21.
//

#include "FunctionType.h"
#include "ObjectType.h"

FunctionType::FunctionType(const VectorOfTypes& parameterTypes, TypeNode* returnType) {

    for (auto* p: parameterTypes) {
        assert(p != nullptr);
    }
    assert(returnType != nullptr);
    this->param_types = parameterTypes;
    this->return_type = returnType;
    this->kind = Kind::FUNCTION;
}

FunctionType* FunctionType::clone() const {
    VectorOfTypes aux;
    for (auto* p: this->param_types) {
        aux.emplace_back(p->clone());
    }
    return new FunctionType(aux, this->return_type->clone());
}

FunctionType::~FunctionType() {
    delete this->return_type;
    for (auto* p: this->param_types) {
        delete p;
    }
}

std::string FunctionType::to_string() const {
    const auto& ftype = *this;
    std::string parameters;
    std::string ret;
    for (auto* ptr: ftype.param_types) {
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
    for (auto* ptr: ftype.param_types) {
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
    for (auto* t: this->param_types) {
        if (t->is_generic()) {
            return true;
        }
    }
    return this->return_type->is_generic();
}

nlohmann::json FunctionType::to_json() {
    nlohmann::json j;
    j["kind"] = "function";
    std::vector<nlohmann::json> v;
    for (auto t: this->param_types) {
        v.push_back(t->to_json());
    }
    j["parameter_types"] = v;
    j["return_type"] = this->return_type->to_json();
    return j;
}