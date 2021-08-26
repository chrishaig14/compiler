//
// Created by chris on 26/4/21.
//

#include "MatchExpressionNode.h"

MatchExpressionNode::MatchExpressionNode(Node* exp, std::vector<std::string> ids, std::vector<std::pair<TypeNode*, ast::Block*>> cases,
                                         TextPosition start, TextPosition end) : Node(NodeType::MATCH_EXP, start, end) {
    this->cases = cases;
    this->exp = exp;
    this->ids = ids;
}

bool MatchExpressionNode::equal(const Node& other) const {
    return false;
}

nlohmann::json MatchExpressionNode::to_json() const {
    return nlohmann::json();
}
