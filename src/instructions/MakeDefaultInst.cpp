//
// Created by chris on 8/8/20.
//

#include "MakeDefaultInst.h"

bool MakeDefaultInst::equal(const Instruction* inst) const {
    return false;
}

std::string MakeDefaultInst::to_string() const {
    return std::string();
}

void MakeDefaultInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
