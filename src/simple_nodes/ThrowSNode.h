//
// Created by chris on 19/6/21.
//

#ifndef XLANG_THROWSNODE_H
#define XLANG_THROWSNODE_H


#include "SNode.h"
#include <vector>
#include <string>

class ThrowSNode : public SNode {
public:
    SNode* expression;
    std::vector<std::string> reachables;
    ThrowSNode(SNode* expression);
};


#endif //XLANG_THROWSNODE_H
