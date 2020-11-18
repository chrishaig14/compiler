//
// Created by chris on 1/8/20.
//

#include "StringNode.h"

void StringNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

StringNode::StringNode(std::string str) : str(str) { this->ntype = STRNG; }

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
