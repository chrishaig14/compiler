//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_BINOPINST_H
#define UNTITLED1_BINOPINST_H


#include "../utils.h"
#include "Instruction.h"

class BinopInst : public Instruction {
    OpType op;
public:
    BinopInst(OpType op);
};


#endif //UNTITLED1_BINOPINST_H
