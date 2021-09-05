//
// Created by chris on 3/7/21.
//

#ifndef XLANG_TYPEOBJECT_H
#define XLANG_TYPEOBJECT_H

#include "Type.h"
#include "Path.h"

class ast::ObjectType : public ast::Type {
public:
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

    Path actual_base_path;
    std::string id;
    ast::VectorOfTypes type_params;
    bool is_generic() const override;
    ast::Type* aliased_type;
};

#endif //XLANG_TYPEOBJECT_H
