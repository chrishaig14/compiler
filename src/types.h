//
// Created by chris on 17/11/20.
//

#ifndef UNTITLED1_TYPES_H
#define UNTITLED1_TYPES_H

#include <vector>
#include <map>

class TypeNode;

class Node;

class Instruction;

typedef std::vector<TypeNode*> VectorOfTypes;
typedef std::vector<Node*> VectorOfNodes;
typedef std::vector<std::string> VectorOfStrings;
typedef std::vector<Instruction*> Code;
typedef std::pair<std::string, TypeNode*> FieldInfo;
typedef std::vector<FieldInfo> StructFields;
typedef std::vector<std::pair<std::string, Instruction*>> CodeLabel;
typedef std::map<std::string, std::map<std::string, Code>> StructProtos;
#endif //UNTITLED1_TYPES_H
