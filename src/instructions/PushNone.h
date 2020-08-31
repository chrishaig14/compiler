//
// Created by chris on 30/8/20.
//

#ifndef UNTITLED1_PUSHNONE_H
#define UNTITLED1_PUSHNONE_H


#include "Instruction.h"
#include "InstructionVisitor.h"

class PushNone : public Instruction {
public:
    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

};


#endif //UNTITLED1_PUSHNONE_H
