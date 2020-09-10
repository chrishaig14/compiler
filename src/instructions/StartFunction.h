//
// Created by chris on 23/8/20.
//

#ifndef UNTITLED1_STARTFUNCTION_H
#define UNTITLED1_STARTFUNCTION_H


#include "Instruction.h"

class StartFunction : public Instruction {
public:
    std::string name;

    StartFunction(const std::string& name);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

};


#endif //UNTITLED1_STARTFUNCTION_H
