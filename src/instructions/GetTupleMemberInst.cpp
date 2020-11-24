//
// Created by chris on 23/11/20.
//

#include "GetTupleMemberInst.h"

bool GetTupleMemberInst::equal(const Instruction* inst) const {
    return false;
}

std::string GetTupleMemberInst::to_string() const {
    return "GET_TUPLE_MEMBER " + std::to_string(this->member);
}

void GetTupleMemberInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
