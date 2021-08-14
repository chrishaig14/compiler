//
// Created by chris on 1/8/20.
//

#ifndef TypeNode_H
#define TypeNode_H


#include <string>
#include <vector>
#include "Node.h"

#include "../types.h"
#include <iostream>


enum class Kind {
    OBJECT, FUNCTION, UNKNOWN
};

class ObjectType;

class FunctionType;

class UnknownTypeNode;

class Class;

class TypeNode {
public:
    Kind kind;

    virtual std::string to_string() const = 0;
    virtual std::string actual_to_string() const = 0;

    virtual bool equal(const TypeNode& other) const = 0;

    virtual TypeNode* clone() const = 0;

    virtual ObjectType& object();

    virtual FunctionType& function();

    virtual const FunctionType& function() const;

    virtual const ObjectType& object() const;

    bool operator==(const TypeNode& other) const;

    bool operator!=(const TypeNode& other) const;

    virtual ~TypeNode() = default;

    bool generic;
    bool is_generic_param;

    virtual nlohmann::json to_json() const = 0;

    virtual bool is_generic() const;
};

std::ostream& operator<<(std::ostream& o, const TypeNode& t);

#endif //TypeNode_H
