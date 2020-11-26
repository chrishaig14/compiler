//
// Created by chris on 25/11/20.
//

#include "PushFloatInst.h"

bool PushFloatInst::equal(const Instruction* inst) const {
    return false;
}

std::string PushFloatInst::to_string() const {
    return "PUSH_FLOAT " + std::to_string(this->value);
}

void PushFloatInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

PushFloatInst::PushFloatInst(float f) {
    this->value = f;
}
