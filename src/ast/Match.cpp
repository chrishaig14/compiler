//
// Created by chris on 26/4/21.
//

#include "Match.h"
using namespace ast;
Match::Match(Node* exp, std::vector<std::string> ids, std::vector<std::pair<TypeNode*, ast::Block*>> cases,
                                         TextPosition start, TextPosition end) : ast::Node(NodeType::MATCH_EXP, start, end) {
    this->cases = cases;
    this->exp = exp;
    this->ids = ids;
}

bool Match::equal(const ast::Node& other) const {
    return false;
}

nlohmann::json Match::to_json() const {
    return nlohmann::json();
}
