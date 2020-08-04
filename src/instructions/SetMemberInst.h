//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_SETMEMBERINST_H
#define UNTITLED1_SETMEMBERINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class SetMemberInst : public Instruction {
public:
    SetMemberInst(const std::string& member);

    bool equal(const Instruction* inst) const {
        const SetMemberInst* other = dynamic_cast<const SetMemberInst*>(inst);
        if (other == nullptr) return false;
        return this->member == other->member;
    }

    void accept(InstructionVisitor& visitor);

    std::string to_string() const {
        return "SET_MEMBER " + this->member;
    }

private:
    std::string member;
};


#endif //UNTITLED1_SETMEMBERINST_H
