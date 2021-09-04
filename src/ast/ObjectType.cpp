//
// Created by chris on 3/7/21.
//

#include "ObjectType.h"

ObjectType::ObjectType(const std::string& identifier, const ast::VectorOfTypes& typeParameters)
        : id(identifier), type_params(typeParameters) {
    for (auto* p: typeParameters) {
        assert(p != nullptr);
    }
    this->kind = Kind::OBJECT;
    this->aliased_type = nullptr;
    this->is_generic_param = false;
}

ast::TypeNode* ObjectType::clone() const {
    ast::VectorOfTypes aux;
    for (auto* p: this->type_params) {
        aux.emplace_back(p->clone());
    }

    auto* n = new ObjectType(this->id, aux);
    n->actual_base_path = this->actual_base_path;
    n->is_generic_param = this->is_generic_param;
    n->aliased_type = this->aliased_type;
    return n;
}

ObjectType::~ObjectType() {
    for (auto* p: this->type_params) {
        delete p;
    }
}

std::string ObjectType::to_string() const {
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

std::string ObjectType::actual_to_string() const {
    const auto& otype = *this;
    std::string parameters;
    for (auto* ptr: otype.type_params) {
        auto& p = *ptr;
        parameters += p.actual_to_string() + ", ";
    }
    if (!parameters.empty()) {
        parameters = parameters.substr(0, parameters.size() - 2);
        return this->actual_base_path.as_str() + "[" + parameters + "]";
    }
    return this->actual_base_path.as_str();
}

bool ObjectType::equal(const ast::TypeNode& other) const {
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
    for (auto* t: this->type_params) {
        if (t->is_generic()) {
            return true;
        }
    }
    return false;
}

nlohmann::json ObjectType::to_json() const {
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

