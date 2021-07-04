//
// Created by chris on 3/7/21.
//

#ifndef XLANG_OBJECTTYPE_H
#define XLANG_OBJECTTYPE_H

#include "TypeNode.h"
#include "Path.h"

class ObjectType : public TypeNode {
public:
    ObjectType(const std::string& identifier, const VectorOfTypes& typeParameters);

    explicit ObjectType(const std::string& identifier);

    bool equal(const TypeNode& other) const override;

    std::string to_string() const override;
    std::string actual_to_string() const override;

    TypeNode* clone() const override;

    ObjectType& object() override;

    const ObjectType& object() const override;

    nlohmann::json to_json() override;

    ~ObjectType() override;

    Path actual_base_path;
    std::string id;
    VectorOfTypes type_params;
    bool is_generic() const override;
    TypeNode* aliased_type;
};

#endif //XLANG_OBJECTTYPE_H
