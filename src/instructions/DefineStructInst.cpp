//
// Created by chris on 8/8/20.
//

#include "DefineStructInst.h"

bool DefineStructInst::equal(const Instruction* inst) const {
    return false;
}

std::string DefineStructInst::to_string() const {
    return "DEFINE_STRUCT " + this->identifier;
}

void DefineStructInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

DefineStructInst::DefineStructInst(const std::string& identifier, const std::map<std::string, Code>& fields)
        : identifier(identifier), fields(fields) {}
