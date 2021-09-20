//
// Created by chris on 16/8/20.
//

#include <cassert>
#include "While.h"

using namespace ast;

While::While(ast::UExpNode& condition, std::unique_ptr<ast::Block>& body, TextPosition start, TextPosition end) : ast::CommonNode(
        CommonNodeType::WHIL,
        start,
        end), body(std::move(body)), condition(std::move(condition)) {
    // assert(condition != nullptr);
    // assert(body != nullptr);
}

bool While::equal(const ast::CommonNode& x) const {
    const auto& other = (While&) x;
    if ((this->body == nullptr && other.body != nullptr) || (this->body != nullptr && other.body == nullptr)) {
        return false;
    }

    return *this->condition == *other.condition &&
           ((this->body == nullptr && other.body == nullptr) || *this->body == *other.body);
}

While::~While() {
    // delete this->body;
    // delete this->condition;
}

nlohmann::json While::to_json() const {
    return {{"type",  "while"},
            {"while", {{"condition", this->condition->to_json()}, {"body", this->body->to_json()}}}};
}


