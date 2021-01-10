//
// Created by chris on 2/8/20.
//

#ifndef BINOPINST_H
#define BINOPINST_H


#include "Instruction.h"
#include "InstructionVisitor.h"
#include "InstructionVisitor.h"
#include "../nodes/optypes.h"

class BinopInst : public Instruction {
public:
    BinopInst(OpType op);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor);

    OpType op;
};


#endif //BINOPINST_H
