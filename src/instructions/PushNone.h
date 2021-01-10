//
// Created by chris on 30/8/20.
//

#ifndef PUSHNONE_H
#define PUSHNONE_H


#include "Instruction.h"
#include "InstructionVisitor.h"

class PushNone : public Instruction {
public:
    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

};


#endif //PUSHNONE_H
