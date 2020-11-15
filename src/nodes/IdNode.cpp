//
// Created by chris on 1/8/20.
//

#include "IdNode.h"

void IdNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

IdNode::IdNode(std::string identifier) : identifier(identifier) {}

bool IdNode::equal(NodeContainer other) const {
//    auto other_ptr = dynamic_cast<IdNode*>(other);
//    if (other_ptr == nullptr) return false;
//    return *this == *other_ptr;
    return false;
}

bool IdNode::operator==(const IdNode& other) const {
    return this->identifier == other.identifier;
}

bool IdNode::operator!=(const IdNode& other) const {return !(*this == other);}
