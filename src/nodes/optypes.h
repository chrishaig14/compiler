//
// Created by chris on 18/11/20.
//

#ifndef UNTITLED1_OPTYPES_H
#define UNTITLED1_OPTYPES_H

enum class OpType {
    ADD, SUB, MUL, DIV, EQ, AND, OR, LEQ, GEQ, LT, GT, NEQ, MOD
};
std::string op_to_string(OpType op);

#endif //UNTITLED1_OPTYPES_H
