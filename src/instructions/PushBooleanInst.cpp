//
// Created by chris on 5/8/20.
//

#include "PushBooleanInst.h"

bool PushBooleanInst::equal(const Instruction* inst) const {
    return false;
}

std::string PushBooleanInst::to_string() const {
    return "PUSHB " + this->boolean ? "True" : "False";
}

void PushBooleanInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
