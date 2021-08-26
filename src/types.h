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

class Node;

class BlockNode;

class NumberNode;

// class BinopNode;

class Instruction;

class IdNode;

class FunctionNode;

typedef std::unique_ptr<TypeNode> UTypeNode;
typedef std::unique_ptr<FunctionNode> UFunctionNode;
typedef std::unique_ptr<ast::BinopNode> UBinopNode;
typedef std::unique_ptr<IdNode> UIdNode;
typedef std::unique_ptr<BlockNode> UBlockNode;
typedef std::unique_ptr<NumberNode> UNumberNode;
typedef std::unique_ptr<Node> UNode;
typedef std::vector<TypeNode*> VectorOfTypes;
typedef std::vector<UTypeNode> VectorOfUTypes;
typedef std::vector<Node*> VectorOfNodes;
typedef std::vector<UNode> VectorOfNodesU;
typedef std::vector<std::string> VectorOfStrings;
typedef std::vector<Instruction*> Code;
typedef std::pair<std::string, TypeNode*> FieldInfo;
typedef std::vector<FieldInfo> StructFields;
typedef std::vector<std::pair<std::string, Instruction*>> CodeLabel;
typedef std::unordered_map<std::string, std::unordered_map<std::string, Code>> StructProtos;
#endif //UNTITLED1_TYPES_H
