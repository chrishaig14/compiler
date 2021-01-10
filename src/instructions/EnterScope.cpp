//
// Created by chris on 18/8/20.
//

#include "EnterScope.h"

bool EnterScope::equal(const Instruction* inst) const {
    auto ptr = dynamic_cast<const EnterScope*>(inst);
    if (ptr == nullptr) {
        return false;
    }
    return this->name == ptr->name;
}

std::string EnterScope::to_string() const {
    return "ENTER_SCOPE " + this->name;
}

void EnterScope::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

EnterScope::EnterScope(const std::string& name) : name(name) {}
