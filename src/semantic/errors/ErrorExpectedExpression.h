//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERROREXPECTEDEXPRESSION_H
#define XLANG_ERROREXPECTEDEXPRESSION_H


#include "Error.h"

class ErrorExpectedExpression : public Error {
public:
    ErrorExpectedExpression(const Entity& entity, const ast::Node& node);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    std::unique_ptr<Entity> entity;
    const ast::Node& node;
};


#endif //XLANG_ERROREXPECTEDEXPRESSION_H
