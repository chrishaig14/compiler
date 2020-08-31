//
// Created by chris on 30/8/20.
//

#include "PushNone.h"

bool PushNone::equal(const Instruction* inst) const {
    auto ptr = dynamic_cast<const PushNone*>(inst);
    return ptr != nullptr;
}

std::string PushNone::to_string() const {
    return "PUSH_NONE";
}

void PushNone::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
