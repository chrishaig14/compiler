//
// Created by chris on 3/7/21.
//

#ifndef XLANG_FUNCTIONTYPE_H
#define XLANG_FUNCTIONTYPE_H


#include "TypeNode.h"

class FunctionType : public TypeNode {
public:
    FunctionType(VectorOfTypes parameterTypes, TypeNode* returnType);

    bool equal(const TypeNode& other) const override;

    std::string to_string() const override;
    std::string actual_to_string() const override;

    FunctionType* clone() const override;

    FunctionType& function() override;

    ~FunctionType() override;

    const FunctionType& function() const override;

    VectorOfTypes param_types;
    TypeNode* return_type;

    bool is_generic() const override;

    nlohmann::json to_json() const override;
};

#endif //XLANG_FUNCTIONTYPE_H
