//
// Created by chris on 1/8/20.
//

#include "StringNode.h"

void StringNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

StringNode::StringNode(std::string str) : str(str) {}

bool StringNode::equal(Node* other) const {
    auto other_ptr = dynamic_cast<StringNode*>(other);
    if (other_ptr == nullptr) return false;
    return *this == *other_ptr;
}

bool StringNode::operator==(StringNode& other) const {
    return this->str == other.str;
}
