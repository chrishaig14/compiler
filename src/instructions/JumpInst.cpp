//
// Created by chris on 17/8/20.
//

#include "JumpInst.h"

bool JumpInst::equal(const Instruction* inst) const {
    auto p = dynamic_cast<const JumpInst*>(inst);
    if (p == nullptr) return false;
    return this->offset == p->offset;
}

std::string JumpInst::to_string() const {
    return "JUMP " + std::to_string(this->offset);
}

void JumpInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

JumpInst::JumpInst(int offset) : offset(offset) {}
