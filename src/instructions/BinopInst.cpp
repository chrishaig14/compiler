//
// Created by chris on 2/8/20.
//

#include "BinopInst.h"

BinopInst::BinopInst(OpType op) {
    this->op = op;
}

bool BinopInst::equal(const Instruction* inst) const {
    const BinopInst* other = dynamic_cast<const BinopInst*> (inst);
    if (other == nullptr) {
        return false;
    }
    return this->op == other->op;
}

std::string BinopInst::to_string() const {
    return op_to_string(this->op);
}

void BinopInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
