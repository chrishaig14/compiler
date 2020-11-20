//
// Created by chris on 2/8/20.
//

#ifndef SETINST_H
#define SETINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class SetInst : public Instruction {
public:
    SetInst(const std::string& identifier);

    bool equal(const Instruction* inst) const {
        const SetInst* other = dynamic_cast<const SetInst*>(inst);
        if (other == nullptr) {
            return false;
        }
        return this->identifier == other->identifier;
    }

    void accept(InstructionVisitor& visitor);

    std::string to_string() const {
        return "SET " + this->identifier;
    }

    std::string identifier;

};


#endif //SETINST_H
