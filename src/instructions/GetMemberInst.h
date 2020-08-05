//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_GETMEMBERINST_H
#define UNTITLED1_GETMEMBERINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class GetMemberInst : public Instruction {
public:
    GetMemberInst(const std::string& member);

    std::string member;
private:

    bool equal(const Instruction* inst) const {
        const GetMemberInst* other = dynamic_cast<const GetMemberInst*>(inst);
        if (other == nullptr) return false;
        return this->member == other->member;
    }

    std::string to_string() const {
        return "GET_MEMBER " + this->member;
    }

    void accept(InstructionVisitor& visitor);


};


#endif //UNTITLED1_GETMEMBERINST_H
