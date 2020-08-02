//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_PUSHSTRINGINST_H
#define UNTITLED1_PUSHSTRINGINST_H


#include <string>
#include "Instruction.h"

class PushStringInst : public Instruction {
public:
    PushStringInst(const std::string& str);

private:
    std::string str;

};


#endif //UNTITLED1_PUSHSTRINGINST_H
