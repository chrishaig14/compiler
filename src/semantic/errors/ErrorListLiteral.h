//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORLISTLITERAL_H
#define XLANG_ERRORLISTLITERAL_H


#include "Error.h"
#include "../../ast/ObjectType.h"

class ErrorListLiteral : public Error {
public:
    ErrorListLiteral(const ast::TypeNode& node, const ObjectType& type, TextPosition position, const ast::Node& node_1);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str() const override;

    const ast::TypeNode& node;
    const ObjectType& type;
    TextPosition position;
    const ast::Node& node_1;
};


#endif //XLANG_ERRORLISTLITERAL_H
