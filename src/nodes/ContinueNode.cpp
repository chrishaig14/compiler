//
// Created by chris on 7/11/20.
//

#include "ContinueNode.h"

void ContinueNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool ContinueNode::equal(Node* other) const {
    return false;
}
