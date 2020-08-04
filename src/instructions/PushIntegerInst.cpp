//
// Created by chris on 2/8/20.
//

#include "PushIntegerInst.h"

PushIntegerInst::PushIntegerInst(int num) : num(num) {}

void PushIntegerInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
