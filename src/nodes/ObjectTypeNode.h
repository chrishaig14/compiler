//
// Created by chris on 15/11/20.
//

#ifndef UNTITLED1_OBJECTTYPENODE_H
#define UNTITLED1_OBJECTTYPENODE_H


#include <string>
#include "TypeNode.h"

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



#endif //UNTITLED1_OBJECTTYPENODE_H
