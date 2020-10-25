//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_TYPENODE_H
#define UNTITLED1_TYPENODE_H


#include <string>
#include <vector>
#include "Node.h"
#include "Visitor.h"

class TypeNode {
public:

    virtual bool equal(TypeNode* other) const = 0;

    virtual std::string to_string() = 0;

    virtual bool is_function() {
        return false;
    }

    json to_json() {
        return json();
    }
};

class FunctionTypeNode : public TypeNode {
public:
    FunctionTypeNode(const std::vector<TypeNode*>& parameterTypes, TypeNode* returnType);

    bool equal(TypeNode* other) const override;

    bool is_function() override { return true; }

    bool operator==(const FunctionTypeNode& other) const {
        if (this->parameter_types.size() != other.parameter_types.size()) return false;
        for (int i = 0; i < this->parameter_types.size(); i++) {
            if (!this->parameter_types[i]->equal(other.parameter_types[i])) return false;
        }
        return this->return_type->equal(other.return_type);
    }

    std::string to_string() override;

    std::vector<TypeNode*> parameter_types;
    TypeNode* return_type;
};

class ObjectTypeNode : public TypeNode {
public:
    ObjectTypeNode(const std::string& identifier, const std::vector<TypeNode*>& typeParameters);

    bool equal(TypeNode* other) const override;

    std::string to_string() override;

    bool operator==(const ObjectTypeNode& other) const {
        if (this->identifier != other.identifier) return false;
        if (this->type_parameters.size() != other.type_parameters.size()) return false;
        for (int i = 0; i < this->type_parameters.size(); i++) {
            if (!this->type_parameters[i]->equal(other.type_parameters[i])) return false;
        }
        return true;
    }

    std::string identifier;
    std::vector<TypeNode*> type_parameters;
};


#endif //UNTITLED1_TYPENODE_H
