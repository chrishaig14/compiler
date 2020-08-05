//
// Created by chris on 5/8/20.
//

#include "MakeListInst.h"

bool MakeListInst::equal(const Instruction* inst) const {
    return false;
}

std::string MakeListInst::to_string() const {
    return "MAKE_LIST " + std::to_string(this->length);
}

void MakeListInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

MakeListInst::MakeListInst(int length) {
    this->length = length;
}
