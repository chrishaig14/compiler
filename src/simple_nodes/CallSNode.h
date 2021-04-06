//
// Created by chris on 4/4/21.
//

#ifndef XLANG_CALLSNODE_H
#define XLANG_CALLSNODE_H


#include <vector>
#include "SNode.h"

class CallSNode : public SNode {
public:
    SNode* function;
    std::vector<SNode*> arguments;
    CallSNode();
};


#endif //XLANG_CALLSNODE_H
