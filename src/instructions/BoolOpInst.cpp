//
// Created by chris on 25/11/20.
//

#include "BoolOpInst.h"

bool BoolOpInst::equal(const Instruction* inst) const {
    return false;
}

std::string BoolOpInst::to_string() const {
    return "BOOL_OP " + bool_op_to_string(this->op);
}

void BoolOpInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

BoolOpInst::BoolOpInst(BoolOp op) {
    this->op = op;
}
