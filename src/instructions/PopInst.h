//
// Created by chris on 31/10/20.
//

#ifndef POPINST_H
#define POPINST_H


#include "Instruction.h"

class PopInst : public Instruction {
public:
    PopInst() {
    }

public:
    bool equal(const Instruction* inst) const override {
        return true;
    }

    std::string to_string() const override {
        std::string st = "POP";
        return st;
    }

    void accept(InstructionVisitor& visitor) override {
        visitor.visit(*this);
    }
};

#endif //POPINST_H
