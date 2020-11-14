//
// Created by chris on 1/8/20.
//

#ifndef TYPENODE_H
#define TYPENODE_H


#include <string>
#include <vector>
#include "Node.h"
#include "Visitor.h"

enum class Kind {
    OBJECT, FUNCTION, UNINITIALIZED
};


std::string otype_to_string(ObjectTypeNode& otype);
bool otype_equal(ObjectTypeNode& a, ObjectTypeNode& b);
bool ftype_equal(FunctionTypeNode& a, FunctionTypeNode& b);
std::string ftype_to_string(FunctionTypeNode& ftype);

class TypeNode {
public:
    ObjectTypeNode* otype;
    FunctionTypeNode* ftype;
    Kind kind;

    TypeNode() {
        this->otype = nullptr;
        this->ftype = nullptr;
        this->kind = Kind::UNINITIALIZED;
    }

    TypeNode(ObjectTypeNode& otype) {
        this->otype = &otype;
        this->ftype = nullptr;
        this->kind = Kind::OBJECT;
    }

    TypeNode(FunctionTypeNode& ftype) {
        this->ftype = &ftype;
        this->otype = nullptr;
        this->kind = Kind::FUNCTION;
    }

    std::string to_string() {
        switch (this->kind) {
            case Kind::OBJECT:
                return otype_to_string(*this->otype);
            case Kind::FUNCTION:
                return ftype_to_string(*this->ftype);
        }
        return "";
    }

    bool operator==(const TypeNode& other) const {
        if (this->kind != other.kind) return false;
        switch (this->kind) {
            case Kind::OBJECT:
                return otype_equal(*this->otype, *other.otype);
            case Kind::FUNCTION:
                return ftype_equal(*this->ftype, *other.ftype);
        }
        return true;
    }

    bool operator!=(const TypeNode& other) const {
        return !(*this == other);
    }
};

class ObjectTypeNode {
public:
    ObjectTypeNode(const std::string& identifier,
                   const std::vector<TypeNode>& typeParameters);

    bool operator==(ObjectTypeNode& other) {
        return otype_equal(*this, other);
    }

    bool operator!=(ObjectTypeNode& other) {
        return !(*this == other);
    }

    std::string to_string() {
        return otype_to_string(*this);
    }

    std::string identifier;
    std::vector<TypeNode> type_parameters;
};


class FunctionTypeNode {
public:
    FunctionTypeNode(const std::vector<TypeNode>& parameterTypes, TypeNode returnType);

    bool is_object() const { return false; }

    bool is_function() { return true; }

    bool operator==(FunctionTypeNode& other) {
        return ftype_equal(*this, other);
    }

    bool operator!=(FunctionTypeNode& other) {
        return !(*this == other);
    }

    std::string to_string() {
        return ftype_to_string(*this);
    }

    std::vector<TypeNode> parameter_types;
    TypeNode return_type;
};


#endif //TYPENODE_H
