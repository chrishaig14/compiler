//
// Created by chris on 15/11/20.
//

#include "ObjectTypeNode.h"
ObjectTypeNode::ObjectTypeNode(const std::string& identifier,
                               const std::vector<TypeNode>& typeParameters) : identifier(
        identifier), type_parameters(typeParameters) {

}
