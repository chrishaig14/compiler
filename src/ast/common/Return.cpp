//
// Created by chris on 1/8/20.
//

#include "Return.h"

using namespace ast;

Return::Return(ast::UExpNode& expression, TextPosition start, TextPosition end) : ast::Node(NodeType::RETRN, start, end),
                                                                          expression(std::move(expression)) {
}

bool Return::equal(const ast::Node& x) const {
    return *((Return&) x).expression == *this->expression;
}

Return::~Return() {
    // for (auto r: this->reachables) {
    //     delete r.second;
    // }
    // if (this->expression != nullptr) {
    //     delete this->expression;
    // }
}

nlohmann::json Return::to_json() const {
    nlohmann::json j;
    j["type"] = "return";
    j["return"] = {{"expression", this->expression != nullptr ? this->expression->to_json() : nlohmann::json()}};
    return j;
}
