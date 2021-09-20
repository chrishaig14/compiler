//
// Created by chris on 17/8/20.
//

#include "Boolean.h"

using namespace ast;

Boolean::Boolean(bool value, TextPosition start, TextPosition end) : ast::ExpNode(ExpNodeType::BOOLEAN, start, end),
                                                                     value(value) {
}

bool Boolean::operator!=(const Boolean& other) const {
    return !(*this == other);
}

bool Boolean::equal(const ast::ExpNode& x) const {
    auto& other = (Boolean&) x;
    return this->value == other.value;
}

nlohmann::json Boolean::to_json() const {
    nlohmann::json j;
    j["type"] = "boolean";
    j["boolean"]["value"] = this->value;
    return j;
}
