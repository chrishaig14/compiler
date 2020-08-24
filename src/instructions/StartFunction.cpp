//
// Created by chris on 23/8/20.
//

#include "StartFunction.h"

bool StartFunction::equal(const Instruction* inst) const {
    return false;
}

std::string StartFunction::to_string() const {
    return "START_FUNCTION " + this->name;
}

void StartFunction::accept(InstructionVisitor& visitor) {

}

StartFunction::StartFunction(const std::string& name) : name(name) { this->type = InstType::START_FUNCTION; }
