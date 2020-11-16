//
// Created by chris on 1/8/20.
//

#include "StringNode.h"

void StringNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

StringNode::StringNode(std::string str) : str(str) {}

bool StringNode::operator==(const StringNode& other) const {
    return this->str == other.str;
}
bool StringNode::operator!=(const StringNode& other) const {
    return !(*this == other);
}
