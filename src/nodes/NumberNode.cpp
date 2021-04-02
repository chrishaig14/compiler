//
// Created by chris on 1/8/20.
//

#include "NumberNode.h"

NumberNode::NumberNode(NumberType num_type, std::string str, TextPosition start, TextPosition end):Node(NUMBER, start, end) {
    this->num_type = num_type;
    this->str = str;
}

bool NumberNode::equal(const Node& x) const {
    auto& other = x.number();
    return this->str == other.str && this->num_type == other.num_type;
}

NumberNode& NumberNode::number() {
    return *this;
}

const NumberNode& NumberNode::number() const {
    return *this;
}

json NumberNode::to_json() const {
    json j;
    j["node"] = "number";
    // j["number"] = this->num;
    return j;
}
