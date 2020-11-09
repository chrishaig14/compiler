//
// Created by chris on 2/8/20.
//

#ifndef PUSHINTEGERINST_H
#define PUSHINTEGERINST_H

#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class PushIntegerInst : public Instruction {
public:
    PushIntegerInst(int num);

    bool equal(const Instruction* inst) const {
        const PushIntegerInst* other = dynamic_cast<const PushIntegerInst*>(inst);
        if (other == nullptr) return false;
        return this->num == other->num;
    }

    std::string to_string() const {
        return "PUSH_INTEGER " + std::to_string(this->num);
    }

    void accept(InstructionVisitor& visitor);

    int num;
};


#endif //PUSHINTEGERINST_H
