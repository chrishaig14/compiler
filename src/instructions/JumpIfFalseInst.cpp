//
// Created by chris on 5/8/20.
//

#include "JumpIfFalseInst.h"

bool JumpIfFalseInst::equal(const Instruction* inst) const {
    return false;
}




std::string JumpIfFalseInst::to_string() const {
    return "JUMP_IF_FALSE " + std::to_string(this->offset);
}

void JumpIfFalseInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
