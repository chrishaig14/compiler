//
// Created by chris on 17/8/20.
//

#ifndef JUMPINST_H
#define JUMPINST_H


#include "Instruction.h"

class JumpInst : public Instruction {
public:

    std::string label;

    JumpInst(std::string label);

    JumpInst(int offset);

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

    bool equal(const Instruction* inst) const override;

    int offset;
};


#endif //JUMPINST_H
