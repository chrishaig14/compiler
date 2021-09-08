//
// Created by chris on 3/7/21.
//

#include <cassert>
#include "TypeObject.h"
#include "../ast/ObjectType.h"

using namespace sem;

TypeObject::TypeObject(const std::string& identifier, const sem::VectorOfTypes& typeParameters)
        : id(identifier), type_params(typeParameters) {
    for (auto* p: typeParameters) {
        assert(p != nullptr);
    }
    this->kind = Kind::OBJECT;
    this->data.aliased_type = nullptr;
    this->is_generic_param = false;
}

TypeObject::TypeObject(const std::string& identifier, const sem::VectorOfTypes& typeParameters, Path actual_base_path)
: id(identifier), type_params(typeParameters) {
    for (auto* p: typeParameters) {
        assert(p != nullptr);
    }
    this->kind = Kind::OBJECT;
    this->data.aliased_type = nullptr;
    this->is_generic_param = false;
    this->data.actual_base_path = actual_base_path;
}

sem::Type* TypeObject::clone() const {
    sem::VectorOfTypes aux;
    for (auto* p: this->type_params) {
        aux.emplace_back(p->clone());
    }

    auto* n = new sem::TypeObject(this->id, aux);
    n->data.actual_base_path = this->data.actual_base_path;
    n->is_generic_param = this->is_generic_param;
    n->data.aliased_type = this->data.aliased_type;
    return n;
}

ast::Type* TypeObject::to_ast() const {
    ast::VectorOfTypes aux;
    for (auto* p: this->type_params) {
        aux.emplace_back(p->to_ast());
    }

    auto* n = new ast::ObjectType(this->id, aux);
    n->data.actual_base_path = this->data.actual_base_path;
    n->is_generic_param = this->is_generic_param;
    n->data.aliased_type = this->data.aliased_type != nullptr ? this->data.aliased_type->to_ast() : nullptr;
    return n;
}

TypeObject::~TypeObject() {
    for (auto* p: this->type_params) {
        delete p;
    }
}

std::string TypeObject::to_string() const {
    const auto& otype = *this;
    std::string parameters;
    for (auto* ptr: otype.type_params) {
        auto& p = *ptr;
        parameters += p.to_string() + ", ";
    }
    if (!parameters.empty()) {
        parameters = parameters.substr(0, parameters.size() - 2);
        return otype.id + "[" + parameters + "]";
    }
    return otype.id + (this->is_generic_param ? "(gen)" : "");
}

std::string TypeObject::actual_to_string() const {
    const auto& otype = *this;
    std::string parameters;
    for (auto* ptr: otype.type_params) {
        auto& p = *ptr;
        parameters += p.actual_to_string() + ", ";
    }
    if (!parameters.empty()) {
        parameters = parameters.substr(0, parameters.size() - 2);
        return this->data.actual_base_path.as_str() + "[" + parameters + "]";
    }
    return this->data.actual_base_path.as_str();
}

bool TypeObject::equal(const sem::Type& other) const {
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

const sem::TypeObject& TypeObject::object() const {
    return *this;
}

sem::TypeObject& TypeObject::object() {
    return *this;
}

TypeObject::TypeObject(const std::string& identifier) : TypeObject(identifier, sem::VectorOfTypes{}) {
    this->is_generic_param = false;
}

TypeObject::TypeObject(const std::string& identifier, Path p) : TypeObject(identifier, sem::VectorOfTypes{}) {
    this->is_generic_param = false;
    this->data.actual_base_path = p;
}

bool TypeObject::is_generic() const {
    if (this->is_generic_param) {
        return true;
    }
    for (auto* t: this->type_params) {
        if (t->is_generic()) {
            return true;
        }
    }
    return false;
}

nlohmann::json TypeObject::to_json() const {
    nlohmann::json j;
    j["kind"] = "object";
    j["id"] = this->id;
    std::vector<nlohmann::json> v;
    for (auto t: this->type_params) {
        v.push_back(t->to_json());
    }
    j["type_params"] = v;
    return j;
}

