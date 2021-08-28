//
// Created by chris on 1/8/20.
//

#include "Number.h"
#include "../json/json.hpp"
using namespace ast;
Number::Number(NumberType num_type, std::string str, TextPosition start, TextPosition end):Node(NodeType::NUMBER, start, end) {
    this->num_type = num_type;
    this->str = str;
}

bool Number::equal(const ast::Node& x) const {
    auto& other = (Number&)x;
    return this->str == other.str && this->num_type == other.num_type;
}

nlohmann::json Number::to_json() const {
    nlohmann::json r;
    r["type"] = "number";
    r["number"]["str"] = this->str;
    r["number"]["num_type"] =
            num_type == NumberType::INTEGER ? "integer" : num_type == NumberType::FLOAT ? "float" : "double";
    return r;
}

