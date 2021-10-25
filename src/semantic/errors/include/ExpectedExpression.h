//
// Created by chris on 12/8/21.
//

#ifndef XLANG_EXPECTEDEXPRESSION_H
#define XLANG_EXPECTEDEXPRESSION_H


#include "Error.h"

class error::ExpectedExpression : public error::Error {
public:
    ExpectedExpression(const Entity& entity, const ast::ExpNode& node);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    std::unique_ptr<Entity> entity;
    const ast::ExpNode& node;
};


#endif //XLANG_EXPECTEDEXPRESSION_H
