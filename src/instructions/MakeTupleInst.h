//
// Created by chris on 23/11/20.
//

#ifndef UNTITLED1_MAKETUPLEINST_H
#define UNTITLED1_MAKETUPLEINST_H


#include "Instruction.h"

class MakeTupleInst : public Instruction {
public:

    MakeTupleInst(int length) { this->length = length; }

    bool equal(const Instruction* inst) const override;
    std::string to_string() const override;
    void accept(InstructionVisitor& visitor) override;

    int length;
};


#endif //UNTITLED1_MAKETUPLEINST_H
