//
// Created by chris on 1/8/20.
//

#include "String.h"
#include "../../json/json.hpp"

using namespace ast;

String::String(const std::string& str, TextPosition start, TextPosition end) : ast::ExpNode(ExpNodeType::STRNG, start, end),
                                                                               str(str) {
}

bool String::equal(const ast::ExpNode& x) const {
    const auto& other = (String&) x;
    return this->str == other.str;
}

nlohmann::json String::to_json() const {
    nlohmann::json j;
    j["type"] = "string";
    j["string"]["str"] = this->str;
    return j;
}

