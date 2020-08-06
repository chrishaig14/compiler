//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_BINOPINST_H
#define UNTITLED1_BINOPINST_H


#include "../utils.h"
#include "Instruction.h"
#include "InstructionVisitor.h"
#include "InstructionVisitor.h"

class BinopInst : public Instruction {
public:
    BinopInst(OpType op);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor);

    OpType op;
};


#endif //UNTITLED1_BINOPINST_H
