//
// Created by chris on 17/8/20.
//

#include "JumpInst.h"

bool JumpInst::equal(const Instruction* inst) const {
    auto p = dynamic_cast<const JumpInst*>(inst);
    if (p == nullptr) return false;
    return this->offset == p->offset && this->label == p->label;
}

std::string JumpInst::to_string() const {
    return "JUMP " + (this->offset != 0 ? std::to_string(this->offset) : this->label);
}

void JumpInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

JumpInst::JumpInst(int offset) {
    this->offset = offset;
}

JumpInst::JumpInst(std::string label) {
    this->label = label;
    this->offset = 0;
}
