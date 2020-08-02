//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_SETMEMBERINST_H
#define UNTITLED1_SETMEMBERINST_H


#include <string>
#include "Instruction.h"

class SetMemberInst : public Instruction{
public:
    SetMemberInst(const std::string& member);

private:
    std::string member;
};


#endif //UNTITLED1_SETMEMBERINST_H
