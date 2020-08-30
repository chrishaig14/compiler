//
// Created by chris on 30/8/20.
//

#include "NoneNode.h"

void NoneNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool NoneNode::equal(Node* other) const {
    auto ptr = dynamic_cast<NoneNode*>(other);
    return ptr != nullptr;
}
