//
// Created by chris on 27/4/21.
//

#include "Match.h"

using namespace sem;

Match::Match(USNode exp, std::string varname, std::vector<std::pair<int, Block*>> cases)
        : SNode(SNodeType::MATCH), exp(std::move(exp)) {
    this->cases = cases;
    this->varname = varname;
}
