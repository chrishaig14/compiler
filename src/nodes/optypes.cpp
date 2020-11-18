//
// Created by chris on 18/11/20.
//
#include <string>
#include "optypes.h"

std::string op_to_string(OpType op) {
    switch (op) {
        case OpType::ADD:
            return "ADD";
        case OpType::SUB:
            return "SUB";
        case OpType::MUL:
            return "MUL";
        case OpType::DIV:
            return "DIV";
        case OpType::EQ:
            return "EQ";
        case OpType::AND:
            return "AND";
        case OpType::OR:
            return "OR";
        case OpType::LEQ:
            return "CMP <=";
        case OpType::GEQ:
            return "CMP >=";
        case OpType::LT:
            return "CMP <";
        case OpType::GT:
            return "CMP >";
        case OpType::NEQ:
            return "CMP !=";
        case OpType::MOD:
            return "MOD";
        default:
            return "OP";
    }
}
