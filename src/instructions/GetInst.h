//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_GETINST_H
#define UNTITLED1_GETINST_H


#include <string>
#include "Instruction.h"

class GetInst : public Instruction {
public:
    GetInst(const std::string& identifier);

public:
    std::string identifier;
};


#endif //UNTITLED1_GETINST_H
