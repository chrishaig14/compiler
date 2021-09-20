//
// Created by chris on 1/8/20.
//

#ifndef TypeNode_H
#define TypeNode_H


#include <string>
#include <vector>
#include "../common/CommonNode.h"

#include "../../util/types.h"
#include "../../simple_nodes/common/include/Type.h"
#include <iostream>


enum class Kind {
    OBJECT, FUNCTION, UNKNOWN
};

class UnknownTypeNode;

class Class;

class ast::Type {
protected:
    explicit Type(Kind kind);
public:
    const Kind kind;

    virtual std::string to_string() const = 0;
    virtual std::string actual_to_string() const = 0;

    virtual bool equal(const ast::Type& other) const = 0;

    virtual ast::Type* clone() const = 0;

    virtual sem::Type* to_sem() const = 0;

    virtual ast::ObjectType& object();

    virtual ast::FunctionType& function();

    virtual const ast::FunctionType& function() const;

    virtual const ast::ObjectType& object() const;

    bool operator==(const ast::Type& other) const;

    bool operator!=(const ast::Type& other) const;

    virtual ~Type() = default;

    bool generic;
    bool is_generic_param;

    virtual nlohmann::json to_json() const = 0;

    virtual bool is_generic() const;
};

std::ostream& operator<<(std::ostream& o, const ast::Type& t);

#endif //TypeNode_H
