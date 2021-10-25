//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ENUMNOVALUE_H
#define XLANG_ENUMNOVALUE_H


#include "Error.h"
#include <ast/expressions/include/Member.h>

class error::EnumNoValue : public error::Error {
public:
    EnumNoValue(std::string a, const std::string b, const ast::Member& node, const Enum& p_enum);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    std::string enum_name;
    const std::string value_name;
    const ast::Member& node;
    const Enum& p_enum;
};


#endif //XLANG_ENUMNOVALUE_H
