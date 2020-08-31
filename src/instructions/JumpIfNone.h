//
// Created by chris on 30/8/20.
//

#ifndef UNTITLED1_JUMPIFNONE_H
#define UNTITLED1_JUMPIFNONE_H


#include "Instruction.h"

class JumpIfNone : public Instruction {
public:
    JumpIfNone(std::string label);

    JumpIfNone(int offset);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

    std::string label;
    int offset;
};


#endif //UNTITLED1_JUMPIFNONE_H
