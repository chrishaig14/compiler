//
// Created by chris on 2/8/20.
//

#include "GetSubscriptInst.h"

void GetSubscriptInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
