//
// Created by chris on 18/8/20.
//

#ifndef UNTITLED1_ENTERSCOPE_H
#define UNTITLED1_ENTERSCOPE_H


#include "Instruction.h"

class EnterScope : public Instruction{
public:
    std::string name;

    EnterScope(const std::string& name);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

};


#endif //UNTITLED1_ENTERSCOPE_H
