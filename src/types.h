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

typedef std::unique_ptr<TypeNode> UTypeNode;
typedef std::unique_ptr<ast::Function> UFunctionNode;
typedef std::unique_ptr<ast::Binop> UBinopNode;
typedef std::unique_ptr<ast::Id> UIdNode;
typedef std::unique_ptr<ast::Block> UBlockNode;
typedef std::unique_ptr<ast::Number> UNumberNode;
typedef std::unique_ptr<ast::Node> UNode;
typedef std::vector<TypeNode*> VectorOfTypes;
typedef std::vector<UTypeNode> VectorOfUTypes;
typedef std::vector<ast::Node*> VectorOfNodes;
typedef std::vector<UNode> VectorOfNodesU;
typedef std::vector<std::string> VectorOfStrings;
typedef std::pair<std::string, TypeNode*> FieldInfo;
typedef std::vector<FieldInfo> StructFields;
#endif //UNTITLED1_TYPES_H
