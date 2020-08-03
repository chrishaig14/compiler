//
// Created by chris on 2/8/20.
//

#include "GetMemberInst.h"

GetMemberInst::GetMemberInst(const std::string& member) : member(member) {}

void GetMemberInst::visit(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
