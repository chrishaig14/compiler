//
// Created by chris on 5/8/20.
//

#ifndef UNTITLED1_PUSHBOOLEANINST_H
#define UNTITLED1_PUSHBOOLEANINST_H


#include "Instruction.h"

class PushBooleanInst : public Instruction {
public:

    PushBooleanInst(bool boolean){
        this->boolean = boolean;
    }

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

    bool boolean;
};


#endif //UNTITLED1_PUSHBOOLEANINST_H
