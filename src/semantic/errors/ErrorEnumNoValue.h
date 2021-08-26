//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORENUMNOVALUE_H
#define XLANG_ERRORENUMNOVALUE_H


#include "Error.h"
#include "../../ast/MemberNode.h"

class ErrorEnumNoValue : public Error {
public:
    ErrorEnumNoValue(std::string a, const std::string b, const MemberNode& node, Enum* p_enum);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str() const override;

    std::string enum_name;
    const std::string value_name;
    const MemberNode& node;
    Enum* p_enum;
};


#endif //XLANG_ERRORENUMNOVALUE_H
