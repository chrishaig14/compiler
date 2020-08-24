//
// Created by chris on 3/8/20.
//

#include "CodeUser.h"

size_t CodeUser::size() {
    return this->code.size();
}

Instruction* CodeUser::get(int i) {
    return this->code[i];
}

CodeUser::CodeUser(Code code) {
    this->code = code;
}