//
// Created by chris on 19/8/20.
//

#ifndef NOPINST_H
#define NOPINST_H


#include "Instruction.h"

class NopInst : public Instruction {
public:
    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

};


#endif //NOPINST_H
