//
// Created by chris on 2/8/20.
//

#ifndef CALLINST_H
#define CALLINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class CallInst : public Instruction {
public:
    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor);
};


#endif //CALLINST_H
