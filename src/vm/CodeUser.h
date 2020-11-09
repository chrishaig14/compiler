//
// Created by chris on 3/8/20.
//

#ifndef CODEUSER_H
#define CODEUSER_H

#include "../utils.h"
#include "Object.h"

class CodeUser {
public:
    Code code;

    CodeUser(Code code);

    Instruction* get(int i);

    size_t size();
};

#endif //CODEUSER_H
