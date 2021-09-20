//
// Created by chris on 26/4/21.
//

#include "Match.h"

using namespace ast;

Match::Match(ExpNode* exp, std::vector<std::string> ids, std::vector<std::pair<ast::UTypeNode, ast::UBlock>> cases,
             TextPosition start, TextPosition end) : ast::Statement(StatementType::MATCH_EXP, start, end),
                                                     cases(std::move(cases)) {
    // this->cases = cases;
    this->exp = exp;
    this->ids = ids;
}

bool Match::equal(const ast::Statement& other) const {
    return false;
}

nlohmann::json Match::to_json() const {
    return nlohmann::json();
}
