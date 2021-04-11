//
// Created by chris on 11/4/21.
//

#ifndef XLANG_CLASSSNODE_H
#define XLANG_CLASSSNODE_H


#include <string>
#include "SNode.h"
#include "FunctionSNode.h"

class ClassSNode : public SNode {
public:
    std::string identifier;
    VectorOfStrings members;

    ClassSNode() : SNode(SNodeType::CLASS) {
    }
};


#endif //XLANG_CLASSSNODE_H
