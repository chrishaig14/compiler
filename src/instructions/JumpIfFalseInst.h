//
// Created by chris on 5/8/20.
//

#ifndef UNTITLED1_JUMPIFFALSEINST_H
#define UNTITLED1_JUMPIFFALSEINST_H


#include "Instruction.h"

class JumpIfFalseInst : public Instruction {
public:
    JumpIfFalseInst(int offset){
        this->offset = offset;
    }

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

    int offset;
};


#endif //UNTITLED1_JUMPIFFALSEINST_H
