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

class TypeNode {
public:
    Kind kind;

    virtual std::string to_string() const = 0;

    virtual bool equal(const TypeNode& other) const = 0;

    virtual TypeNode* clone() const = 0;

    virtual ObjectType& object();

    virtual FunctionType& function();

    virtual const FunctionType& function() const;

    virtual const ObjectType& object() const;

    bool operator==(const TypeNode& other) const;

    bool operator!=(const TypeNode& other) const;

    virtual ~TypeNode() {}

};

class ObjectType : public TypeNode {
public:
    ObjectType(const std::string
                   & identifier,
               const VectorOfTypes& typeParameters
    );

    bool equal(const TypeNode& other) const override;

    std::string to_string() const override;

    TypeNode* clone() const override;

    ObjectType& object() override;

    const ObjectType& object() const override;

    ~ObjectType();

    std::string id;
    VectorOfTypes type_parameters;
};

class UnknownTypeNode : public TypeNode {
public:
    UnknownTypeNode() {
        this->kind = Kind::UNKNOWN;
    }

    std::string to_string() const override {
        return std::string();
    }

    bool equal(const TypeNode& other) const override {
        return false;
    }

    TypeNode* clone() const override {
        return new UnknownTypeNode();
    }
};

class FunctionType : public TypeNode {
public:
    FunctionType(const VectorOfTypes& parameterTypes, TypeNode* returnType);

    bool equal(const TypeNode& other) const override;

    std::string to_string() const override;

    FunctionType* clone() const override;

    FunctionType& function() override;

    ~FunctionType();

    const FunctionType& function() const override;

    VectorOfTypes parameter_types;
    TypeNode* return_type;
};


#endif //TypeNode_H
