//
// Created by chris on 27/4/21.
//

#ifndef XLANG_MATCHSNODE_H
#define XLANG_MATCHSNODE_H


#include <vector>
#include "SNode.h"
#include "Block.h"
#include <string>

class sem::Match : public sem::SNode {
public:
    SNode* exp;
    std::vector<std::pair<int, Block*>> cases;
    std::string varname;
    Match(SNode* exp, std::string varname, std::vector<std::pair<int, Block*>> cases);

    bool equals(const SNode& o) const override {
        // auto& other = (const Declaration&) o;
        // return this->identifier == other.identifier && *this->expression == *other.expression;
        return false;
    }
};


#endif //XLANG_MATCH_H
