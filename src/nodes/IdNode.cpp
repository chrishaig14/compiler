//
// Created by chris on 1/8/20.
//

#include "IdNode.h"

void IdNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

IdNode::IdNode(std::string identifier) : identifier(identifier) { this->ntype = ID; }


bool IdNode::equal(const Node& x) const {
    auto& other = x.id();
    return this->identifier == other.identifier;
}


IdNode& IdNode::id() {
    return *this;
}

const IdNode& IdNode::id() const {
    return *this;
}
