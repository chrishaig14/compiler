//
// Created by chris on 2/8/20.
//

#include "DeclareInst.h"

DeclareInst::DeclareInst(std::string identifier) : identifier(identifier) {}

bool DeclareInst::equal(const Instruction* inst) const {
    const DeclareInst* other = dynamic_cast<const DeclareInst*>(inst);
    if (other == nullptr) return false;
    return this->identifier == other->identifier;
}

std::string DeclareInst::to_string() const {
    return "DECLARE " + this->identifier;
}

void DeclareInst::visit(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
