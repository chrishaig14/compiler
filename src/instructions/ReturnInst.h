//
// Created by chris on 2/8/20.
//

#ifndef RETURNINST_H
#define RETURNINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class ReturnInst : public Instruction {
    bool equal(const Instruction* inst) const {
        const ReturnInst* other = dynamic_cast<const ReturnInst*>(inst);
        if (other == nullptr) {
            return false;
        }
        return true;
    }

    void accept(InstructionVisitor& visitor);

public:
    std::string to_string() const {
        return "RETURN";
    }
};


#endif //RETURNINST_H
