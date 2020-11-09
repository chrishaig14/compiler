//
// Created by chris on 5/8/20.
//

#ifndef JUMPIFFALSEINST_H
#define JUMPIFFALSEINST_H


#include "Instruction.h"

class JumpIfFalseInst : public Instruction {
public:
    JumpIfFalseInst(std::string label);

    JumpIfFalseInst(int offset);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

    std::string label;
    int offset;
};


#endif //JUMPIFFALSEINST_H
