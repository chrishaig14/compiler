//
// Created by chris on 5/8/20.
//

#include "PushBooleanInst.h"

bool PushBooleanInst::equal(const Instruction* inst) const {
    const PushBooleanInst* other = dynamic_cast<const PushBooleanInst*>(inst);
    if (other == nullptr) {
        return false;
    }
    return *this == *other;
}

std::string PushBooleanInst::to_string() const {
    return "PUSHB " + std::string(this->boolean ? "True" : "False");
}

void PushBooleanInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

bool PushBooleanInst::operator==(const PushBooleanInst& other) const {
    return this->boolean == other.boolean;
}
