//
// Created by chris on 18/8/20.
//

#include "BreakNode.h"

void BreakNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool BreakNode::equal(NodeContainer other) const {
//    BreakNode* o = dynamic_cast<BreakNode*>(other);
//    return o != nullptr;
return false;
}
