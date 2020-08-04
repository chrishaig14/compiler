//
// Created by chris on 2/8/20.
//

#include "ReturnInst.h"

void ReturnInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
