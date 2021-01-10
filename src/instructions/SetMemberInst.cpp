//
// Created by chris on 2/8/20.
//

#include "SetMemberInst.h"

SetMemberInst::SetMemberInst(const std::string& member) : member(member) {}

void SetMemberInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
