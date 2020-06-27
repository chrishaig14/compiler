//
// Created by chris on 15/6/20.
//

#ifndef UNTITLED1_REPLACER_H
#define UNTITLED1_REPLACER_H

#include <gtest/gtest.h>
#include "AstNode.h"

typedef std::vector<TypeNode*> VectorOfTypes;
typedef std::vector<std::string> VectorOfStrings;
typedef std::vector<AstNode*> VectorOfNodes;

void replace_types(AstNode* node, VectorOfStrings params, VectorOfTypes replacements);

void replace_types(TypeNode* node, VectorOfStrings params, VectorOfTypes replacements);

void replace_types(VectorOfNodes block, VectorOfStrings params, VectorOfTypes replacements);

void replace_types(DeclarationNode* node, VectorOfStrings params, VectorOfTypes replacements);

void replace_types(ClassNode* node, VectorOfStrings params, VectorOfTypes replacements);


#endif //UNTITLED1_REPLACER_H
