//
// Created by chris on 19/8/20.
//

#include "NopInst.h"

bool NopInst::equal(const Instruction* inst) const {
    auto ptr = dynamic_cast<const NopInst*>(inst);
    return ptr != nullptr;
}

std::string NopInst::to_string() const {
    return "NOP";
}

void NopInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
