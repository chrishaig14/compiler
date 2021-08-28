//
// Created by chris on 1/8/20.
//

#include "Assignment.h"
#include "../json/json.hpp"

using namespace ast;

Assignment::Assignment(UNode& lvalue, UNode& rvalue, TextPosition start, TextPosition end) : ast::Node(NodeType::ASSIGN,
                                                                                                       start,
                                                                                                       end),
                                                                                             lvalue(std::move(lvalue)),
                                                                                             rvalue(std::move(rvalue)) {
}

bool Assignment::equal(const ast::Node& x) const {
    auto& other = (Assignment&) x;
    return *this->lvalue == *other.lvalue && *this->rvalue == *other.rvalue;
}

Assignment::~Assignment() {
    // delete this->lvalue;
    // delete this->rvalue;
}

nlohmann::json Assignment::to_json() const {
    nlohmann::json j;
    j["type"] = "assignment";
    j["assignment"]["lvalue"] = this->lvalue->to_json();
    j["assignment"]["rvalue"] = this->rvalue->to_json();
    return j;
}

