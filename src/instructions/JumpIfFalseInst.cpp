//
// Created by chris on 5/8/20.
//

#include "JumpIfFalseInst.h"

bool JumpIfFalseInst::equal(const Instruction* inst) const {
    const JumpIfFalseInst* other_ptr = dynamic_cast<const JumpIfFalseInst*>(inst);
    if (other_ptr == nullptr) return false;
    return this->offset == other_ptr->offset && this->label == other_ptr->label;
}


std::string JumpIfFalseInst::to_string() const {
    return "JUMP_IF_FALSE " + (this->offset != 0 ? std::to_string(this->offset) : this->label);
}

void JumpIfFalseInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

JumpIfFalseInst::JumpIfFalseInst(int offset) {
    this->offset = offset;
}

JumpIfFalseInst::JumpIfFalseInst(std::string label) {
    this->label = label;
    this->offset = 0;
}
