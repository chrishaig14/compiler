//
// Created by chris on 3/7/21.
//

#ifndef XLANG_TypeObject_H
#define XLANG_TypeObject_H

#include "Type.h"
#include "../../sem.h"
#include <common/Path.h>
#include <json/json.hpp>
#include <set>

class sem::ObjectTypeData {
public:
    Path actual_base_path;
    sem::Type* aliased_type;
};


class sem::TypeObject : public sem::Type {
public:
    TypeObject(const std::string& identifier, Path p);
    TypeObject(const std::string& identifier, const VectorOfTypes& typeParameters, Path actual_base_path);
    ObjectTypeData data;
    TypeObject(const std::string& identifier, const sem::VectorOfTypes& typeParameters);

    explicit TypeObject(const std::string& identifier);

    TypeObject(const TypeObject& other)  : Type(other) {
        this->id = other.id;
        this->typeclasses = other.typeclasses;
        this->data.actual_base_path = other.data.actual_base_path;
        this->data.aliased_type = other.data.aliased_type != nullptr ? other.data.aliased_type->clone() : nullptr;
        for (auto& tp: other.type_params) {
            this->type_params.push_back(tp->clone());
        }
        this->generic = other.generic;
        this->is_generic_param = other.is_generic_param;
        this->kind = Kind::OBJECT;
    }

    bool equal(const sem::Type& other) const override;

    std::string to_string() const override;
    std::string actual_to_string() const override;


    sem::Type* clone() const override;
    ast::Type* to_ast() const override;

    sem::TypeObject& object() override;

    const sem::TypeObject& object() const override;

    nlohmann::json to_json() const override;

    ~TypeObject() override;

    // Path actual_base_path;
    std::string id;
    sem::VectorOfTypes type_params;
    bool is_generic() const override;
    // sem::Type* aliased_type;
    void add_typeclass(std::string typeclass_name);
    std::set<std::string> typeclasses;
};

#endif //XLANG_TypeObject_H
