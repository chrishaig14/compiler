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

class MatchCase {
public:
    int index;
    std::string var_name;
    sem::Block body;

    MatchCase(int index, std::string var_name, const sem::Block& body) : index(index), var_name(var_name), body(body) {
    }

    MatchCase(const MatchCase& other) : body(other.body) {
        this->index = other.index;
        this->var_name = other.var_name;
    }
};

class sem::Match : public sem::Common {
public:
    UExp exp;
    std::vector<MatchCase> cases;
    Match(UExp exp, std::vector<MatchCase> cases);
    Match(const Match& other);
    bool equals(const Common& o) const override;
    std::unique_ptr<Common> clone() const override;
};


#endif //XLANG_MATCH_H
