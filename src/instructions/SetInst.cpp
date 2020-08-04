//
// Created by chris on 2/8/20.
//

#include "SetInst.h"

SetInst::SetInst(const std::string& identifier) : identifier(identifier) {}

void SetInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
