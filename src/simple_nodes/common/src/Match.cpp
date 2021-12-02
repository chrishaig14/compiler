//
// Created by chris on 27/4/21.
//

#include "../include/Match.h"

using namespace sem;

Match::Match(UExp exp, std::string varname, std::vector<std::pair<int, sem::Block>> cases)
        : Common(CommonType::MATCH), exp(std::move(exp)), cases(cases) {
    this->varname = varname;
}

bool Match::equals(const sem::Common& o) const {
    // auto& other = (const Declaration&) o;
    // return this->identifier == other.identifier && *this->expression == *other.expression;
    return false;
}

std::unique_ptr<Common> Match::clone() const {
    return std::make_unique<Match>(*this);
}

Match::Match(const Match& other) : sem::Common(CommonType::MATCH),cases(other.cases){
    this->exp = other.exp->clone();
    this->varname = other.varname;
}
