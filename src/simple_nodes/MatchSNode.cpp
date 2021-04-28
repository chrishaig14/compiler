//
// Created by chris on 27/4/21.
//

#include "MatchSNode.h"

MatchSNode::MatchSNode(SNode* exp, std::string varname, std::vector<std::pair<int, BlockSNode*>> cases) : SNode(
        SNodeType::MATCH) {
    this->exp = exp;
    this->cases = cases;
    this->varname = varname;
}
