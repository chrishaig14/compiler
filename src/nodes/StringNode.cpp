//
// Created by chris on 1/8/20.
//

#include "StringNode.h"

StringNode::StringNode(std::string str, TextPosition start) : str(str) {
    this->ntype = STRNG;
    this->start = start;
}

bool StringNode::equal(const Node& x) const {
    auto& other = x.strng();
    return this->str == other.str;
}

StringNode& StringNode::strng() {
    return *this;
}

const StringNode& StringNode::strng() const {
    return *this;
}

json StringNode::to_json() const {
    json j;
    j["node"] = "string";
    j["str"] = this->str;
    return j;
}
