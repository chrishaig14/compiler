//
// Created by chris on 15/11/20.
//

#ifndef UNTITLED1_FUNCTIONTYPENODE_H
#define UNTITLED1_FUNCTIONTYPENODE_H

#include "../utils.h"

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

    std::string to_string() const {
        return ftype_to_string(*this);
    }

    std::vector<TypeNode> parameter_types;
    TypeNode return_type;
};


#endif //UNTITLED1_FUNCTIONTYPENODE_H
