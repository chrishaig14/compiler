//
// Created by chris on 4/4/21.
//

#ifndef XLANG_RETURNSNODE_H
#define XLANG_RETURNSNODE_H


#include "SNode.h"
#include <vector>
#include <string>

class ReturnSNode : public SNode {
public:
    USNode expression;
    std::vector<std::string> reachables;
    ReturnSNode(USNode& expression);
};


#endif //XLANG_RETURNSNODE_H
