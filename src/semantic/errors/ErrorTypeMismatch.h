//
// Created by chris on 8/8/21.
//

#ifndef XLANG_ERRORTYPEMISMATCH_H
#define XLANG_ERRORTYPEMISMATCH_H

#include "../../nodes/TypeNode.h"
#include "../../units/Entity.h"
#include "../../nodes/Node.h"
#include "Error.h"

class ErrorTypeMismatch : public Error {
    const TypeNode& expected;
    const Node& value_node;
    const Entity& actual;
public:
    ErrorTypeMismatch(const TypeNode& expected, const Node& value_node, const Entity& actual);

    std::string to_str() const override;
    bool equal(const Error& other) const override;
    Error* clone() const override;
};


#endif //XLANG_ERRORTYPEMISMATCH_H
