//
// Created by chris on 1/8/20.
//

#include "StringNode.h"
#include "../json/json.hpp"

StringNode::StringNode(const std::string& str, TextPosition start, TextPosition end) : Node(NodeType::STRNG, start, end), str(str) {
}

bool StringNode::equal(const Node& x) const {
    const auto& other = (StringNode&)x;
    return this->str == other.str;
}

nlohmann::json StringNode::to_json() {
    nlohmann::json j;
    j["type"] = "string";
    j["string"]["str"] = this->str;
    return j;
}

