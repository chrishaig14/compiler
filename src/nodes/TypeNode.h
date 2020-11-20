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
    OBJECT, FUNCTION
};

class ObjectTypeNode;

class FunctionTypeNode;

class TypeNode {
public:
    Kind kind;

    virtual std::string to_string() const = 0;

    virtual bool equal(const TypeNode& other) const = 0;

    virtual TypeNode* clone() const = 0;

    virtual ObjectTypeNode& object() { throw std::runtime_error("Getting wrong type!"); }

    virtual FunctionTypeNode& function() { throw std::runtime_error("Getting wrong type!"); }

    virtual const FunctionTypeNode& function() const { throw std::runtime_error("Getting wrong type!"); }

    virtual const ObjectTypeNode& object() const { throw std::runtime_error("Getting wrong type!"); }

    bool operator==(const TypeNode& other) const;

    bool operator!=(const TypeNode& other) const;

    virtual ~TypeNode() {}

};

class ObjectTypeNode : public TypeNode {
public:
    ObjectTypeNode(const std::string
                   & identifier,
                   const VectorOfTypes& typeParameters
    );

    bool equal(const TypeNode& other) const override;

    std::string to_string() const override;

    TypeNode* clone() const override;

    ObjectTypeNode& object() override { return *this; }

    const ObjectTypeNode& object() const override { return *this; }

    ~ObjectTypeNode();

    std::string identifier;
    VectorOfTypes type_parameters;
};


class FunctionTypeNode : public TypeNode {
public:
    FunctionTypeNode(const VectorOfTypes& parameterTypes, TypeNode& returnType);

    bool equal(const TypeNode& other) const override;

    std::string to_string() const override;

    FunctionTypeNode* clone() const override;

    FunctionTypeNode& function() override { return *this; }

    ~FunctionTypeNode();

    const FunctionTypeNode& function() const override { return *this; }

    VectorOfTypes parameter_types;
    TypeNode* return_type;
};


#endif //TypeNode_H
