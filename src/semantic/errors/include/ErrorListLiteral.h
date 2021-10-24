//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORLISTLITERAL_H
#define XLANG_ERRORLISTLITERAL_H


#include "Error.h"
#include <ast/general/ObjectType.h>

class ErrorListLiteral : public Error {
public:
    ErrorListLiteral(const sem::Type& node, const sem::TypeObject& type, TextPosition position, const ast::Statement& node_1);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    const sem::Type& node;
    const sem::TypeObject& type;
    TextPosition position;
    const ast::Statement& node_1;
};


#endif //XLANG_ERRORLISTLITERAL_H
