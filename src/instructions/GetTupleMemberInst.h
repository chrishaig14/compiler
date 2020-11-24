//
// Created by chris on 23/11/20.
//

#ifndef UNTITLED1_GETTUPLEMEMBERINST_H
#define UNTITLED1_GETTUPLEMEMBERINST_H


#include "Instruction.h"

class GetTupleMemberInst : public Instruction {
public:
    int member;

    GetTupleMemberInst(int member) { this->member = member; }

    bool equal(const Instruction* inst) const override;
    std::string to_string() const override;
    void accept(InstructionVisitor& visitor) override;

};


#endif //UNTITLED1_GETTUPLEMEMBERINST_H
