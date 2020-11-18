//
// Created by chris on 1/8/20.
//

#ifndef TypeNode_H
#define TypeNode_H


#include <string>
#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "../types.h"

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

    ObjectTypeNode& object() { throw std::runtime_error("Getting wrong type!"); }

    virtual TypeNode* clone() const = 0;

    FunctionTypeNode& function() { throw std::runtime_error("Getting wrong type!"); }

    const FunctionTypeNode& function() const { throw std::runtime_error("Getting wrong type!"); }

    const ObjectTypeNode& object() const { throw std::runtime_error("Getting wrong type!"); }

    bool operator==(const TypeNode& other) const {
        if (this->kind != other.kind) {
            return false;
        }
        return this->equal(other);
    }

    bool operator!=(const TypeNode& other) const {
        return !(*this == other);
    }

};

class ObjectTypeNode : public TypeNode {
public:
    ObjectTypeNode(const std::string
                   & identifier,
                   const VectorOfTypes& typeParameters
    );

    bool equal(const TypeNode& other) const {
        auto& a = *this;
        auto& b = other.object();
        if (a.identifier != b.identifier) {
            return false;
        }
        if (a.type_parameters.size() != b.type_parameters.size()) {
            return false;
        }
        for (int i = 0; i < a.type_parameters.size(); i++) {
            if (a.type_parameters[i] != b.type_parameters[i]) {
                return false;
            }
        }
        return true;
    }

    std::string to_string() const {
        auto& otype = *this;
        std::string parameters;
        for (auto ptr: otype.type_parameters) {
            auto& p = *ptr;
            parameters += p.to_string() + ", ";
        }
        if (parameters.size() != 0) {
            parameters = parameters.substr(0, parameters.size() - 2);
            return otype.identifier + "[" + parameters + "]";
        }
        return otype.identifier;
    }

    TypeNode* clone() const override;

    std::string identifier;
    VectorOfTypes type_parameters;
};


class FunctionTypeNode : public TypeNode {
public:
    FunctionTypeNode(const VectorOfTypes& parameterTypes, TypeNode* returnType);

//    FunctionTypeNode* operator=(const FunctionTypeNode* other) = default;

//    FunctionTypeNode*operator=(const FunctionTypeNode* other) {
//        return FunctionTypeNode*other.parameter_types, other.return_type);
//    }

    bool equal(const TypeNode& other) const override {
        auto& a = *this;
        auto& b = other.function();
        if (a.parameter_types.size() != b.parameter_types.size()) {
            return false;
        }
        for (int i = 0; i < a.parameter_types.size(); i++) {
            if (a.parameter_types[i] != b.parameter_types[i]) {
                return false;
            }
        }
        return a.return_type == b.return_type;
    }

    std::string to_string() const {
        auto& ftype = *this;
        std::string parameters;
        std::string ret;
        for (auto ptr: ftype.parameter_types) {
            auto& p = *ptr;
            parameters += p.to_string() + ", ";
        }
        if (ftype.parameter_types.size() != 0) {
            parameters = parameters.substr(0, parameters.size() - 2);
        }
        ret = ftype.return_type->to_string() + ", ";
        return "fun (" + parameters + ") . " + ret;
    }

    TypeNode* clone() const override;

    VectorOfTypes parameter_types;
    TypeNode* return_type;
};


#endif //TypeNode_H
