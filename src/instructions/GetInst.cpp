//
// Created by chris on 2/8/20.
//

#include "GetInst.h"
#include "../utils.h"

GetInst::GetInst(const std::string& identifier) : identifier(identifier) {}

std::string GetInst::to_string() const {
    return "GET " + this->identifier + " (D: " + std::to_string(this->location.depth) + " I: " +
           std::to_string(this->location.index) + ")";
}

void GetInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

GetInst::GetInst(const std::string& identifier, VariableLocation location) {
    this->identifier = identifier;
    this->location = location;
}
