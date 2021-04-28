//
// Created by chris on 27/4/21.
//

#ifndef XLANG_MATCHSNODE_H
#define XLANG_MATCHSNODE_H


#include <vector>
#include "SNode.h"
#include "BlockSNode.h"
#include <string>

class MatchSNode : public SNode {
public:
    SNode* exp;
    std::vector<std::pair<int, BlockSNode*>> cases;
    std::string varname;
    MatchSNode(SNode* exp, std::string varname, std::vector<std::pair<int, BlockSNode*>> cases);
};


#endif //XLANG_MATCHSNODE_H
