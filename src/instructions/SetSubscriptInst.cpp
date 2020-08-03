//
// Created by chris on 2/8/20.
//

#include "SetSubscriptInst.h"

void SetSubscriptInst::visit(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
