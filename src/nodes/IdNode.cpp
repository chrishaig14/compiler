//
// Created by chris on 1/8/20.
//

#include "IdNode.h"

void IdNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

IdNode::IdNode(std::string identifier) : identifier(identifier) {}


bool IdNode::operator==(const IdNode& other) const {
    return this->identifier == other.identifier;
}

bool IdNode::operator!=(const IdNode& other) const {return !(*this == other);}
