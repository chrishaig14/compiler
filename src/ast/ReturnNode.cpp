//
// Created by chris on 1/8/20.
//

#include "ReturnNode.h"

using namespace ast;

ReturnNode::ReturnNode(UNode& expression, TextPosition start, TextPosition end) : Node(NodeType::RETRN, start, end),
                                                                                  expression(std::move(expression)) {
}

bool ReturnNode::equal(const Node& x) const {
    return *((ReturnNode&) x).expression == *this->expression;
}

ReturnNode::~ReturnNode() {
    // for (auto r: this->reachables) {
    //     delete r.second;
    // }
    // if (this->expression != nullptr) {
    //     delete this->expression;
    // }
}

nlohmann::json ReturnNode::to_json() const {
    nlohmann::json j;
    j["type"] = "return";
    j["return"] = {{"expression", this->expression != nullptr ? this->expression->to_json() : nlohmann::json()}};
    return j;
}
