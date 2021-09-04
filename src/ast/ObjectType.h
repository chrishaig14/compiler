//
// Created by chris on 3/7/21.
//

#ifndef XLANG_OBJECTTYPE_H
#define XLANG_OBJECTTYPE_H

#include "TypeNode.h"
#include "Path.h"

class ast::ObjectType : public ast::TypeNode {
public:
    ObjectType(const std::string& identifier, const ast::VectorOfTypes& typeParameters);

    explicit ObjectType(const std::string& identifier);

    bool equal(const ast::TypeNode& other) const override;

    std::string to_string() const override;
    std::string actual_to_string() const override;

    ast::TypeNode* clone() const override;

    ast::ObjectType& object() override;

    const ast::ObjectType& object() const override;

    nlohmann::json to_json() const override;

    ~ObjectType() override;

    Path actual_base_path;
    std::string id;
    ast::VectorOfTypes type_params;
    bool is_generic() const override;
    ast::TypeNode* aliased_type;
};

#endif //XLANG_OBJECTTYPE_H
