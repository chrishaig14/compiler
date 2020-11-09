//
// Created by chris on 2/8/20.
//

#ifndef DECLAREINST_H
#define DECLAREINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class DeclareInst : public Instruction {
public:
    std::string identifier;

    DeclareInst(std::string identifier);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor);
};


#endif //DECLAREINST_H
