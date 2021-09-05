//
// Created by chris on 3/7/21.
//

#ifndef XLANG_TypeFunction_H
#define XLANG_TypeFunction_H


#include "Type.h"
#include "sem.h"

class sem::TypeFunction : public sem::Type {
public:
    TypeFunction(sem::VectorOfTypes parameterTypes, sem::UType returnType);

    bool equal(const sem::Type& other) const override;

    std::string to_string() const override;
    std::string actual_to_string() const override;

    TypeFunction* clone() const override;

    TypeFunction& function() override;

    ~TypeFunction() override;

    const TypeFunction& function() const override;

    // VectorOfTypes param_types;
    sem::VectorOfUTypes param_types;
    // sem::UType t;
    sem::UType return_type;

    bool is_generic() const override;

    nlohmann::json to_json() const override;
};

#endif //XLANG_TypeFunction_H
