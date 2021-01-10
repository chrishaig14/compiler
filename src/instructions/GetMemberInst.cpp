//
// Created by chris on 2/8/20.
//

#include "GetMemberInst.h"

GetMemberInst::GetMemberInst(const std::string& member) : s_member(member) {}

void GetMemberInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
