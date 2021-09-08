//
// Created by chris on 3/7/21.
//

#ifndef XLANG_OBJECTTYPE_H
#define XLANG_OBJECTTYPE_H

#include "Type.h"
#include "Path.h"

class ObjectTypeData {
public:
    Path actual_base_path;
    ast::Type* aliased_type;
};

class ast::ObjectType : public ast::Type {
public:
    std::string id;
    ast::VectorOfTypes type_params;
    ObjectTypeData data;

    ObjectType(const std::string& identifier, const ast::VectorOfTypes& typeParameters);
    explicit ObjectType(const std::string& identifier);
    bool equal(const ast::Type& other) const override;
    std::string to_string() const override;
    std::string actual_to_string() const override;
    ast::Type* clone() const override;
    ast::ObjectType& object() override;
    const ast::ObjectType& object() const override;
    nlohmann::json to_json() const override;
    ~ObjectType() override;
    bool is_generic() const override;
    sem::Type* to_sem() const override;
};

#endif //XLANG_OBJECTTYPE_H
