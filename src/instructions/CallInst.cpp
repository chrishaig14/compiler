//
// Created by chris on 2/8/20.
//

#include "CallInst.h"

bool CallInst::equal(const Instruction* inst) const {
    const CallInst* other = dynamic_cast<const CallInst*>(inst);
    if (other == nullptr) {
        return false;
    }
    return true;
}

std::string CallInst::to_string() const {
    return "CALL";
}

void CallInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
