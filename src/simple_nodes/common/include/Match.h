//
// Created by chris on 27/4/21.
//

#ifndef XLANG_MATCHSNODE_H
#define XLANG_MATCHSNODE_H


#include <vector>
#include "SNode.h"
#include "Block.h"
#include <string>
#include "../../expressions/include/Exp.h"

class sem::Match : public sem::SNode {
public:
    UExp exp;
    std::vector<std::pair<int, Block*>> cases;
    std::string varname;
    Match(UExp exp, std::string varname, std::vector<std::pair<int, Block*>> cases);

    bool equals(const SNode& o) const override;
};


#endif //XLANG_MATCH_H
