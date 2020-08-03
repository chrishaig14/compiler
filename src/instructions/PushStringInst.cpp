//
// Created by chris on 2/8/20.
//

#include "PushStringInst.h"

PushStringInst::PushStringInst(const std::string& str) : str(str) {}

void PushStringInst::visit(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
