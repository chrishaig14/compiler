//
// Created by chris on 3/7/21.
//

#ifndef XLANG_TYPEFUNCTION_H
#define XLANG_TYPEFUNCTION_H


#include "Type.h"
#include "../../ast/statements/Statement.h"


class ast::FunctionType : public ast::Type {
public:
    FunctionType(ast::VectorOfTypes parameterTypes, ast::UTypeNode returnType);

    bool equal(const ast::Type& other) const override;

    sem::Type* to_sem() const override;

    std::string to_string() const override;
    std::string actual_to_string() const override;

    FunctionType* clone() const override;

    FunctionType& function() override;

    ~FunctionType() override;

    const FunctionType& function() const override;

    // VectorOfTypes param_types;
    ast::VectorOfUTypes param_types;
    // ast::UTypeNode t;
    ast::UTypeNode return_type;

    bool is_generic() const override;

    nlohmann::json to_json() const override;
};

#endif //XLANG_TYPEFUNCTION_H
