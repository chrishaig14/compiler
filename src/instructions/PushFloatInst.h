//
// Created by chris on 25/11/20.
//

#ifndef UNTITLED1_PUSHFLOATINST_H
#define UNTITLED1_PUSHFLOATINST_H


#include "Instruction.h"

class PushFloatInst : public Instruction {
public:
    float value;

    PushFloatInst(float f);

    bool equal(const Instruction* inst) const override;
    std::string to_string() const override;
    void accept(InstructionVisitor& visitor) override;

};


#endif //UNTITLED1_PUSHFLOATINST_H
