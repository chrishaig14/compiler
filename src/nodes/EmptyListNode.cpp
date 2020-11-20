//
// Created by chris on 6/9/20.
//

#include "EmptyListNode.h"

void EmptyListNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}


EmptyListNode::EmptyListNode(TypeNode* type) : type(type) { this->ntype = EMPTYLST; }

EmptyListNode& EmptyListNode::emptylst() {
    return *this;
}

const EmptyListNode& EmptyListNode::emptylst() const {
    return *this;
}

bool EmptyListNode::equal(const Node& other) const {
    return *other.emptylst().type == *this->type;
}

EmptyListNode::~EmptyListNode() {
    delete this->type;
}
