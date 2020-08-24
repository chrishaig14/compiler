//
// Created by chris on 3/8/20.
//

#ifndef UNTITLED1_CODEUSER_H
#define UNTITLED1_CODEUSER_H

#include "../utils.h"
#include "Object.h"

class CodeUser {
public:
    Code code;

    CodeUser(Code code);

    Instruction* get(int i);

    size_t size();
};

#endif //UNTITLED1_CODEUSER_H
