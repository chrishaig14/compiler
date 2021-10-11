//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORUNUSEDRETURNVALUE_H
#define XLANG_ERRORUNUSEDRETURNVALUE_H


#include "Error.h"

class ErrorUnusedReturnValue : public Error {
public:
    ErrorUnusedReturnValue(const Entity& entity, const ast::Call& node);

    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;
    const ast::Call& node;
    std::unique_ptr<Entity> entity;
};


#endif //XLANG_ERRORUNUSEDRETURNVALUE_H
