//
// Created by chris on 27/4/21.
//

#include "Match.h"

Match::Match(SNode* exp, std::string varname, std::vector<std::pair<int, Block*>> cases) : SNode(
        SNodeType::MATCH) {
    this->exp = exp;
    this->cases = cases;
    this->varname = varname;
}
