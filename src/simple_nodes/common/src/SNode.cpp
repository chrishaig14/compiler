//
// Created by chris on 4/4/21.
//

#include "../include/SNode.h"
using namespace sem;
SNode::SNode(SNodeType type) {
    this->type = type;
}

bool SNode::operator!=(const SNode& other) const {
    return !(*this == other);
}

bool SNode::operator==(const SNode& other) const {
    if (this->type != other.type) {
        return false;
    }
    return this->equals(other);
}
