//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERROROBJECTNOSPECIALMETHOD_H
#define XLANG_ERROROBJECTNOSPECIALMETHOD_H


#include "Error.h"
#include "../../../ast/expressions/include/Subscript.h"

class ErrorObjectNoSpecialMethod : public Error {
public:
    ErrorObjectNoSpecialMethod(const sem::Type& type, const std::string& method_name, const ast::Subscript& node);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    const ast::Subscript& node;
    std::string method_name;
    sem::UType type;
};


#endif //XLANG_ERROROBJECTNOSPECIALMETHOD_H
