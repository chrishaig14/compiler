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
    }
    throw std::runtime_error("Unknow op type");
}

std::string bool_op_to_string(BoolOpType op) {
    switch (op) {
        case BoolOpType::AND:
            return "AND";
        case BoolOpType::OR:
            return "OR";
        case BoolOpType::LE:
            return "CMP <=";
        case BoolOpType::GE:
            return "CMP >=";
        case BoolOpType::LT:
            return "CMP <";
        case BoolOpType::GT:
            return "CMP >";
        case BoolOpType::NE:
            return "CMP !=";
        case BoolOpType::EQ:
            return "==";
    }
    throw std::runtime_error("Dont know whta to do with bool op");
}
