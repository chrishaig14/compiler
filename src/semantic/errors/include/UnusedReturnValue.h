//
// Created by chris on 12/8/21.
//

#ifndef XLANG_UNUSEDRETURNVALUE_H
#define XLANG_UNUSEDRETURNVALUE_H


#include "Error.h"

class error::UnusedReturnValue : public error::Error {
public:
    UnusedReturnValue(const Entity& entity, const ast::Call& node);

    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;
    const ast::Call& node;
    std::unique_ptr<Entity> entity;
};


#endif //XLANG_UNUSEDRETURNVALUE_H
