//
// Created by chris on 2/8/20.
//

#ifndef SETINST_H
#define SETINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"
#include "../utils.h"

class SetInst : public Instruction {
public:
    SetInst(const std::string& identifier);
    SetInst(const std::string& identifier, VariableLocation location);
    VariableLocation location;

    bool equal(const Instruction* inst) const {
        const SetInst* other = dynamic_cast<const SetInst*>(inst);
        if (other == nullptr) {
            return false;
        }
        return this->identifier == other->identifier;
    }

    void accept(InstructionVisitor& visitor);

    std::string to_string() const {
        return "SET " + this->identifier + " (D: " + std::to_string(this->location.depth) + " I: " +
               std::to_string(this->location.index) + ")";
    }

    std::string identifier;

};


#endif //SETINST_H
