//
// Created by chris on 1/8/20.
//

#include "NumberNode.h"
#include "../json/json.hpp"
using namespace ast;
NumberNode::NumberNode(NumberType num_type, std::string str, TextPosition start, TextPosition end):Node(NodeType::NUMBER, start, end) {
    this->num_type = num_type;
    this->str = str;
}

bool NumberNode::equal(const ast::Node& x) const {
    auto& other = (NumberNode&)x;
    return this->str == other.str && this->num_type == other.num_type;
}

nlohmann::json NumberNode::to_json() const {
    nlohmann::json r;
    r["type"] = "number";
    r["number"]["str"] = this->str;
    r["number"]["num_type"] =
            num_type == NumberType::INTEGER ? "integer" : num_type == NumberType::FLOAT ? "float" : "double";
    return r;
}

