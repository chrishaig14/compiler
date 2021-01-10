//
// Created by chris on 25/11/20.
//

#ifndef UNTITLED1_BOOLOPINST_H
#define UNTITLED1_BOOLOPINST_H


#include "Instruction.h"
#include "../nodes/optypes.h"

class BoolOpInst : public Instruction {
public:
    BoolOp op;

    BoolOpInst(BoolOp op);

    bool equal(const Instruction* inst) const override;
    std::string to_string() const override;
    void accept(InstructionVisitor& visitor) override;

};


#endif //UNTITLED1_BOOLOPINST_H
