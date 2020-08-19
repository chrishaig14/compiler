//
// Created by chris on 17/8/20.
//

#ifndef UNTITLED1_JUMPINST_H
#define UNTITLED1_JUMPINST_H


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


#endif //UNTITLED1_JUMPINST_H
