//
// Created by chris on 5/8/20.
//

#ifndef PUSHBOOLEANINST_H
#define PUSHBOOLEANINST_H


#include "Instruction.h"

class PushBooleanInst : public Instruction {
public:

    PushBooleanInst(bool boolean) {
        this->boolean = boolean;
    }

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

    bool operator==(const PushBooleanInst& other) const;

    bool boolean;
};


#endif //PUSHBOOLEANINST_H
