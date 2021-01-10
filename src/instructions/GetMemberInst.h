//
// Created by chris on 2/8/20.
//

#ifndef GETMEMBERINST_H
#define GETMEMBERINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class GetMemberInst : public Instruction {
public:
    GetMemberInst(const std::string& member);
    std::string s_member;

    std::string to_string() const {
        return "GET_MEMBER " + this->s_member;
    }

private:

    bool equal(const Instruction* inst) const {
        const GetMemberInst* other = dynamic_cast<const GetMemberInst*>(inst);
        if (other == nullptr) {
            return false;
        }
        return this->s_member == other->s_member;
    }

    void accept(InstructionVisitor& visitor);

};


#endif //GETMEMBERINST_H
