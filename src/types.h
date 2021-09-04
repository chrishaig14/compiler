//
// Created by chris on 17/11/20.
//

#ifndef UNTITLED1_TYPES_H
#define UNTITLED1_TYPES_H

#include <vector>
#include <unordered_map>
#include <memory>

#include "ast/ast.h"

class TypeNode;

class ObjectType;

class FunctionType;

typedef std::unique_ptr<FunctionType> UFunctionType;
typedef std::unique_ptr<ObjectType> UObjectType;
typedef std::unique_ptr<TypeNode> UTypeNode;

typedef std::vector<TypeNode*> VectorOfTypes;
typedef std::vector<UTypeNode> VectorOfUTypes;
typedef std::vector<ast::Node*> VectorOfNodes;
typedef std::vector<std::string> VectorOfStrings;
typedef std::pair<std::string, TypeNode*> FieldInfo;
typedef std::vector<FieldInfo> StructFields;
#endif //UNTITLED1_TYPES_H
