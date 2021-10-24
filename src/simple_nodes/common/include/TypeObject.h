//
// Created by chris on 3/7/21.
//

#ifndef XLANG_TypeObject_H
#define XLANG_TypeObject_H

#include "Type.h"
#include "../../sem.h"
#include "../../../util/Path.h"
#include <json/json.hpp>

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
    std::string typeclass;
};

#endif //XLANG_TypeObject_H
