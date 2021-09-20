//
// Created by chris on 8/8/21.
//

#ifndef XLANG_ERRORTYPEMISMATCH_H
#define XLANG_ERRORTYPEMISMATCH_H

#include "../../../ast/general/Type.h"
#include "../../../units/Entity.h"
#include "../../../ast/exp/include/ExpNode.h"
#include "Error.h"

class ErrorTypeMismatch : public Error {
    ErrorTypeMismatch(const sem::Type& expected, TextPosition start, TextPosition end, const Entity& actual);
    sem::UType expected;
    std::unique_ptr<Entity> actual;
public:
    const TextPosition start;
    const TextPosition end;
    ErrorTypeMismatch(const sem::Type& expected, const ast::ExpNode& value_node, const Entity& actual);

    std::string to_str(const CodeLines& code) const override;
    bool equal(const Error& other) const override;
    Error* clone() const override;
};


#endif //XLANG_ERRORTYPEMISMATCH_H
