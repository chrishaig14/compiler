//
// Created by chris on 5/8/20.
//

#ifndef UNTITLED1_MAKELISTINST_H
#define UNTITLED1_MAKELISTINST_H


#include "Instruction.h"

class MakeListInst : public Instruction {
public:
    MakeListInst(int length);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

    int length;
};


#endif //UNTITLED1_MAKELISTINST_H
