//
// Created by chris on 8/8/20.
//

#ifndef UNTITLED1_MAKEDEFAULTINST_H
#define UNTITLED1_MAKEDEFAULTINST_H


#include "Instruction.h"

class MakeDefaultInst : public Instruction{
public:
    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

    MakeDefaultInst(std::string identifier){
        this->identifier = identifier;
    }

    std::string identifier;
};


#endif //UNTITLED1_MAKEDEFAULTINST_H
