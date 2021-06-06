//
// Created by chris on 26/4/21.
//

#ifndef XLANG_BREAKSNODE_H
#define XLANG_BREAKSNODE_H


#include <vector>
#include <string>
#include "SNode.h"

class BreakSNode : public SNode {
public:
    std::vector<std::string> reachables;
    BreakSNode();
};


#endif //XLANG_BREAKSNODE_H
