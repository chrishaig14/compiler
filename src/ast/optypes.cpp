//
// Created by chris on 18/11/20.
//
#include <string>
#include <stdexcept>
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
        case OpType::MOD:
            return "MOD";
        case OpType::AND:
            return "AND";
        case OpType::OR:
            return "OR";
        case OpType::LE:
            return "CMP <=";
        case OpType::GE:
            return "CMP >=";
        case OpType::LT:
            return "CMP <";
        case OpType::GT:
            return "CMP >";
        case OpType::NE:
            return "CMP !=";
        case OpType::EQ:
            return "==";
    }
    throw std::runtime_error("Unknow op type");
}