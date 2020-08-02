//
// Created by chris on 2/8/20.
//

#include "BlockNode.h"

void BlockNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

