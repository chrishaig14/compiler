//
// Created by chris on 12/8/21.
//

#include "../include/BoolOp.h"

using namespace error;

Error* BoolOp::clone() const {
    return nullptr;
}

bool BoolOp::equal(const Error& other) const {
    return false;
}

std::string BoolOp::to_str(const CodeLines& code) const {
    return std::string();
}

BoolOp::BoolOp(const Entity& left, const Entity& right, TextPosition position):Error(ErrorType::bool_op) {

}
