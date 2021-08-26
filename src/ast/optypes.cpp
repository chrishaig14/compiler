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

std::string bool_op_to_string(BoolOp op) {
    switch (op) {
        case BoolOp::AND:
            return "AND";
        case BoolOp::OR:
            return "OR";
        case BoolOp::LE:
            return "CMP <=";
        case BoolOp::GE:
            return "CMP >=";
        case BoolOp::LT:
            return "CMP <";
        case BoolOp::GT:
            return "CMP >";
        case BoolOp::NE:
            return "CMP !=";
        case BoolOp::EQ:
            return "==";
    }
    throw std::runtime_error("Dont know whta to do with bool op");
}
