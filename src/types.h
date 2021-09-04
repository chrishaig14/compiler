//
// Created by chris on 17/11/20.
//

#ifndef UNTITLED1_TYPES_H
#define UNTITLED1_TYPES_H

#include <vector>
#include <unordered_map>
#include <memory>

#include "ast/ast.h"

class FunctionType;

typedef std::unique_ptr<FunctionType> UFunctionType;

typedef std::vector<ast::Node*> VectorOfNodes;
typedef std::vector<std::string> VectorOfStrings;
#endif //UNTITLED1_TYPES_H
