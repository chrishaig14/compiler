//
// Created by chris on 1/8/20.
//

#include "FunctionNode.h"

void FunctionNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}
