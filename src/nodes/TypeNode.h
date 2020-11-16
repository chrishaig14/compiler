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


std::string otype_to_string(const ObjectTypeNode& otype);
bool otype_equal(const ObjectTypeNode& a, const ObjectTypeNode& b);
bool ftype_equal(const FunctionTypeNode& a, const FunctionTypeNode& b);
std::string ftype_to_string(const FunctionTypeNode& ftype);

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

    TypeNode(ObjectTypeNode* otype) {
        this->otype = otype;
        this->ftype = nullptr;
        this->kind = Kind::OBJECT;
    }

    TypeNode(FunctionTypeNode* ftype) {
        this->ftype = ftype;
        this->otype = nullptr;
        this->kind = Kind::FUNCTION;
    }

    std::string to_string() const {
        switch (this->kind) {
            case Kind::OBJECT:
                return otype_to_string(*this->otype);
            case Kind::FUNCTION:
                return ftype_to_string(*this->ftype);
        }
        return "";
    }

    bool operator==(const TypeNode& other) const {
        if (this->kind != other.kind) {
            return false;
        }
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

    bool operator==(const ObjectTypeNode& other) const {
        return otype_equal(*this, other);
    }

    bool operator!=(const ObjectTypeNode& other) const {
        return !(*this == other);
    }

    std::string to_string() const {
        return otype_to_string(*this);
    }

    std::string identifier;
    std::vector<TypeNode> type_parameters;
};


class FunctionTypeNode {
public:
    FunctionTypeNode(const std::vector<TypeNode>& parameterTypes, TypeNode returnType);

//    FunctionTypeNode& operator=(const FunctionTypeNode& other) = default;

//    FunctionTypeNode operator=(const FunctionTypeNode& other) {
//        return FunctionTypeNode(other.parameter_types, other.return_type);
//    }

    bool operator==(const FunctionTypeNode& other) const {
        return ftype_equal(*this, other);
    }

    bool operator!=(const FunctionTypeNode& other) const {
        return !(*this == other);
    }

    std::string to_string() const {
        return ftype_to_string(*this);
    }

    std::vector<TypeNode> parameter_types;
    TypeNode return_type;
};


#endif //TYPENODE_H
