//
// Created by chris on 23/11/20.
//

#include "MakeTupleInst.h"

bool MakeTupleInst::equal(const Instruction* inst) const {
    return false;
}

std::string MakeTupleInst::to_string() const {
    return "MAKE_TUPLE " + this->length;
}

void MakeTupleInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
