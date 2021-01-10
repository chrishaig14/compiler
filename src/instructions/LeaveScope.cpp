//
// Created by chris on 18/8/20.
//

#include "LeaveScope.h"

bool LeaveScope::equal(const Instruction* inst) const {
    auto ptr = dynamic_cast<const LeaveScope*>(inst);
    if (ptr == nullptr) {
        return false;
    }
    return this->name == ptr->name;
}

std::string LeaveScope::to_string() const {
    return "LEAVE_SCOPE " + this->name;
}

void LeaveScope::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

LeaveScope::LeaveScope(const std::string& name) : name(name) {}
