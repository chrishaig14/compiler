//
// Created by chris on 5/8/20.
//

#ifndef MAKELISTINST_H
#define MAKELISTINST_H


#include "Instruction.h"

class MakeListInst : public Instruction {
public:
    MakeListInst(int length);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

    int length;
};


#endif //MAKELISTINST_H
