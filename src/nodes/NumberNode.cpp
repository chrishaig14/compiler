//
// Created by chris on 1/8/20.
//

#include "NumberNode.h"

NumberNode::NumberNode(int number, TextPosition start) : num(number) {
    this->ntype = NUMBER;
    this->start = start;
}

bool NumberNode::equal(const Node& x) const {
    auto& other = x.number();
    return this->num == other.num;
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
    j["number"] = this->num;
    return j;
}
