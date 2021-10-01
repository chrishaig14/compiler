//
// Created by chris on 27/4/21.
//

#include "../include/Match.h"

using namespace sem;

Match::Match(UExp exp, std::string varname, std::vector<std::pair<int, std::unique_ptr<sem::Block>>> cases) : Common(
        CommonType::MATCH), exp(std::move(exp)), cases(std::move(cases)) {
    this->varname = varname;
}

bool Match::equals(const sem::Common& o) const {
    // auto& other = (const Declaration&) o;
    // return this->identifier == other.identifier && *this->expression == *other.expression;
    return false;
}
