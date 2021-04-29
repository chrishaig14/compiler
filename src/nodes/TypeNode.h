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

class Path {
private:
    VectorOfStrings path_parts;
    std::string s;
public:
    Path() {
    }

    Path(Path p, std::string c) {
        this->path_parts = p.as_vec();
        this->path_parts.push_back(c);
        for (auto cc: this->path_parts) {
            this->s += cc + ".";
        }
        this->s = this->s.substr(0, this->s.size() - 1);
    }

    Path(std::string s) {
        this->s = s;
        size_t k = 0;
        this->path_parts.push_back("");
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] == '.') {
                this->path_parts.push_back("");
                k++;
            } else {
                this->path_parts[k] += s[i];
            }
        }
    }

    Path(VectorOfStrings p) {
        this->path_parts = p;
        for (auto cc: this->path_parts) {
            this->s += cc + ".";
        }
        this->s = this->s.substr(0, this->s.size() - 1);
    }

    std::string as_str() const {
        return this->s;
    }

    VectorOfStrings as_vec() {
        return this->path_parts;
    }
};

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

    virtual ~TypeNode() {
    }

    bool generic;
    bool is_generic_param;

    virtual bool is_generic() const;
};

class ObjectType : public TypeNode {
public:
    ObjectType(const std::string& identifier, const VectorOfTypes& typeParameters);

    ObjectType(const std::string& identifier);

    bool equal(const TypeNode& other) const override;

    std::string to_string() const override;
    std::string actual_to_string() const override;

    TypeNode* clone() const override;

    Class* clazz;

    ObjectType& object() override;

    const ObjectType& object() const override;

    ~ObjectType();

    Path actual_base_path;
    std::string id;
    VectorOfTypes type_params;
    bool is_generic() const override;
    TypeNode* aliased_type;
};

class UnknownTypeNode : public TypeNode {
public:
    UnknownTypeNode() {
        this->kind = Kind::UNKNOWN;
    }

    std::string to_string() const override {
        return std::string();
    }

    std::string actual_to_string() const override {
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
    std::string actual_to_string() const override;

    FunctionType* clone() const override;

    FunctionType& function() override;

    ~FunctionType();

    const FunctionType& function() const override;

    VectorOfTypes param_types;
    TypeNode* return_type;

    bool is_generic() const override;
};


#endif //TypeNode_H
