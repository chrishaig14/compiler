//
// Created by chris on 30/8/20.
//

#include "NoneNode.h"

void NoneNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool NoneNode::equal(NodeContainer other) const {
//    auto ptr = dynamic_cast<NoneNode*>(other);
//    return ptr != nullptr;
    return false;
}

bool NoneNode::operator==(const NoneNode& other) const {return true;}

bool NoneNode::operator!=(const NoneNode& other) const {return false;}
