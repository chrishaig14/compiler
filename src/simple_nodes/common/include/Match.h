//
// Created by chris on 27/4/21.
//

#ifndef XLANG_MATCHSNODE_H
#define XLANG_MATCHSNODE_H


#include <vector>
#include "Common.h"
#include "Block.h"
#include <string>
#include "../../expressions/include/Exp.h"

class sem::Match : public sem::Common {
public:
    UExp exp;
    std::vector<std::pair<int, std::unique_ptr<Block>>> cases;
    std::string varname;
    Match(UExp exp, std::string varname, std::vector<std::pair<int, std::unique_ptr<Block>>> cases);

    bool equals(const Common& o) const override;
};


#endif //XLANG_MATCH_H
