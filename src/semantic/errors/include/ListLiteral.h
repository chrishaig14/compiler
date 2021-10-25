//
// Created by chris on 12/8/21.
//

#ifndef XLANG_LISTLITERAL_H
#define XLANG_LISTLITERAL_H


#include "Error.h"
#include <ast/general/ObjectType.h>

class error::ListLiteral : public error::Error {
public:
    ListLiteral(const sem::Type& node, const sem::TypeObject& type, TextPosition position, const ast::Statement& node_1);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    const sem::Type& node;
    const sem::TypeObject& type;
    TextPosition position;
    const ast::Statement& node_1;
};


#endif //XLANG_LISTLITERAL_H
