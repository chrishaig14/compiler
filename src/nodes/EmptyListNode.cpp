//
// Created by chris on 6/9/20.
//

#include "EmptyListNode.h"

void EmptyListNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool EmptyListNode::equal(NodeContainer other) const {
//    auto ptr = dynamic_cast<EmptyListNode*>(other);
//    if (ptr == nullptr) return false;
//    return this->type->equal(ptr->type);
return false;
}

EmptyListNode::EmptyListNode(TypeNode& type) : type(type) {}
