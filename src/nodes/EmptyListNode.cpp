//
// Created by chris on 6/9/20.
//

#include "EmptyListNode.h"

void EmptyListNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}


EmptyListNode::EmptyListNode(TypeNode* type) : type(type) {}
