//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_SETINST_H
#define UNTITLED1_SETINST_H


#include <string>
#include "Instruction.h"

class SetInst : public Instruction {
public:
    SetInst(const std::string& identifier);

private:
    std::string identifier;

};


#endif //UNTITLED1_SETINST_H
