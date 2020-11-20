//
// Created by chris on 30/8/20.
//

#include "JumpIfNone.h"

bool JumpIfNone::equal(const Instruction* inst) const {
    const JumpIfNone* other_ptr = dynamic_cast<const JumpIfNone*>(inst);
    if (other_ptr == nullptr) {
        return false;
    }
    return this->offset == other_ptr->offset && this->label == other_ptr->label;
}


std::string JumpIfNone::to_string() const {
    return "JUMP_IF_NONE " + (this->offset != 0 ? std::to_string(this->offset) : this->label);
}

void JumpIfNone::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

JumpIfNone::JumpIfNone(int offset) {
    this->offset = offset;
}

JumpIfNone::JumpIfNone(std::string label) {
    this->label = label;
    this->offset = 0;
}
