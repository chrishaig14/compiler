//
// Created by chris on 2/8/20.
//

#ifndef UTILS_H
#define UTILS_H

#include "instructions/Instruction.h"
#include "nodes/nodes.h"

typedef std::vector<std::string> VectorOfStrings;
typedef std::vector<Instruction*> Code;
typedef std::pair<std::string, TypeNode*> FieldInfo;
typedef std::vector<FieldInfo> StructFields;
typedef std::vector<std::pair<std::string, Instruction*>> CodeLabel;
typedef std::map<std::string, std::map<std::string, Code>> StructProtos;


std::ostream& operator<<(std::ostream& out, const CodeLabel v);

template<typename T>
bool item_in_vec(T item, std::vector<T> vec) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
}


#endif //UTILS_H
