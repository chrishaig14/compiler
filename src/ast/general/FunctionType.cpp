//
// Created by chris on 3/7/21.
//

#include <cassert>
#include "TypeFunction.h"
#include <simple_nodes/common/include/TypeFunction.h>
#include "ObjectType.h"

using namespace ast;

FunctionType::FunctionType(ast::VectorOfTypes parameterTypes, ast::UTypeNode returnType)
        : Type(Kind::FUNCTION), return_type(std::move(returnType)) {

    for (auto* p: parameterTypes) {
        assert(p != nullptr);
    }
    // assert(returnType != nullptr);
    // this->param_types = parameterTypes;
    for (auto* p: parameterTypes) {
        this->param_types.push_back(ast::UTypeNode(p));
    }
}

ast::FunctionType* ast::FunctionType::clone() const {
    ast::VectorOfTypes aux;
    for (auto& p: this->param_types) {
        aux.emplace_back(p->clone());
    }
    return new ast::FunctionType(aux, ast::UTypeNode(this->return_type->clone()));
}

FunctionType::~FunctionType() {
    // delete this->return_type;
    // for (auto& p: this->param_types) {
    //     delete p;
    // }
}

std::string FunctionType::to_string() const {
    const auto& ftype = *this;
    std::string parameters;
    std::string ret;
    for (auto& ptr: ftype.param_types) {
        auto& p = *ptr;
        parameters += p.to_string() + ", ";
    }
    if (!ftype.param_types.empty()) {
        parameters = parameters.substr(0, parameters.size() - 2);
    }
    ret = ftype.return_type->to_string();
    return "fun (" + parameters + ")" + (*ftype.return_type == ast::ObjectType(".None") ? "" : (" -> " + ret));
}

std::string ast::FunctionType::actual_to_string() const {
    const auto& ftype = *this;
    std::string parameters;
    std::string ret;
    for (auto& ptr: ftype.param_types) {
        auto& p = *ptr;
        parameters += p.actual_to_string() + ", ";
    }
    if (!ftype.param_types.empty()) {
        parameters = parameters.substr(0, parameters.size() - 2);
    }
    ret = ftype.return_type->actual_to_string();
    return "fun (" + parameters + ")" + (*ftype.return_type == ast::ObjectType(".None") ? "" : (" -> " + ret));
}

bool ast::FunctionType::equal(const ast::Type& other) const {
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

ast::FunctionType& ast::FunctionType::function() {
    return *this;
}

const ast::FunctionType& ast::FunctionType::function() const {
    return *this;
}

bool ast::FunctionType::is_generic() const {
    for (auto& t: this->param_types) {
        if (t->is_generic()) {
            return true;
        }
    }
    return this->return_type->is_generic();
}

nlohmann::json ast::FunctionType::to_json() const {
    nlohmann::json j;
    j["kind"] = "function";
    std::vector<nlohmann::json> v;
    for (auto& t: this->param_types) {
        v.push_back(t->to_json());
    }
    j["parameter_types"] = v;
    j["return_type"] = this->return_type->to_json();
    return j;
}

sem::Type* FunctionType::to_sem() const {
    sem::VectorOfTypes aux;
    for (auto& p: this->param_types) {
        aux.emplace_back(p->to_sem());
    }
    return new sem::TypeFunction(aux, sem::UType(this->return_type->to_sem()));
}
