//
// Created by chris on 23/8/20.
//

#include "EndFunction.h"

bool EndFunction::equal(const Instruction* inst) const {
    return false;
}

std::string EndFunction::to_string() const {
    return "END_FUNCTION " + this->name;
}

void EndFunction::accept(InstructionVisitor& visitor) {

}

EndFunction::EndFunction(const std::string& name) : name(name) { this->type = InstType::END_FUNCTION; }
