//
// Created by chris on 14/8/20.
//

#include <cassert>
#include "For.h"

using namespace ast;

For::For(const std::string& var, ast::UNode exp, ast::UBlock body, TextPosition start, TextPosition end)
        : ast::Node(NodeType::FORLOOP, start, end), _exp(std::move(exp)), _body(std::move(body)), exp(*_exp),
          var(var), body(*_body) {
    // assert(exp != nullptr);
    // assert(body != nullptr);
}

bool For::equal(const ast::Node& x) const {
    const auto& other = (For&) x;
    return this->var == other.var && this->exp == other.exp && this->body == other.body;
}

For::~For() {
    // delete this->exp;
    // delete this->body;
}

nlohmann::json For::to_json() const {
    nlohmann::json j;
    j["type"] = "for";
    j["for"]["var"] = this->var;
    j["for"]["exp"] = this->exp.to_json();
    j["for"]["body"] = this->body.to_json();
    return j;
}

