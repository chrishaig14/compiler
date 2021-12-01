//
// Created by chris on 3/7/21.
//

#include "../include/TypeFunction.h"
#include "../include/TypeObject.h"
#include <ast/general/TypeFunction.h>
#include "../include/TypeObject.h"

using namespace sem;

TypeFunction::TypeFunction(sem::VectorOfTypes parameterTypes, sem::UType returnType)
        : return_type(std::move(returnType)) {

    // for (auto* p: parameterTypes) {
    //     assert(p != nullptr);
    // }
    // assert(returnType != nullptr);
    // this->param_types = parameterTypes;
    for (auto* p: parameterTypes) {
        this->param_types.push_back(sem::UType(p));
    }
    this->kind = Kind::FUNCTION;
}

sem::TypeFunction* sem::TypeFunction::clone() const {
    sem::VectorOfTypes aux;
    for (auto& p: this->param_types) {
        aux.emplace_back(p->clone());
    }
    return new sem::TypeFunction(aux, sem::UType(this->return_type->clone()));
}


ast::Type* sem::TypeFunction::to_ast() const {
    ast::VectorOfTypes aux;
    for (auto& p: this->param_types) {
        aux.emplace_back(p->to_ast());
    }
    return new ast::FunctionType(aux, ast::UTypeNode(this->return_type->to_ast()));
}

TypeFunction::~TypeFunction() {
    // delete this->return_type;
    // for (auto& p: this->param_types) {
    //     delete p;
    // }
}

std::string TypeFunction::to_string() const {
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
    return "fun (" + parameters + ")" + (*ftype.return_type == sem::TypeObject(".None") ? "" : (" -> " + ret));
}

std::string sem::TypeFunction::actual_to_string() const {
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
    return "fun (" + parameters + ")" + (*ftype.return_type == sem::TypeObject(".None") ? "" : (" -> " + ret));
}

bool sem::TypeFunction::equal(const sem::Type& other) const {
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

sem::TypeFunction& sem::TypeFunction::function() {
    return *this;
}

const sem::TypeFunction& sem::TypeFunction::function() const {
    return *this;
}

bool sem::TypeFunction::is_generic() const {
    for (auto& t: this->param_types) {
        if (t->is_generic()) {
            return true;
        }
    }
    return this->return_type->is_generic();
}

nlohmann::json sem::TypeFunction::to_json() const {
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