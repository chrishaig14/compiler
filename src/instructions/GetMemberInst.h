//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_GETMEMBERINST_H
#define UNTITLED1_GETMEMBERINST_H


#include <string>
#include "Instruction.h"

class GetMemberInst : public Instruction {
public:
    GetMemberInst(const std::string& member);

private:
    std::string member;

};


#endif //UNTITLED1_GETMEMBERINST_H
