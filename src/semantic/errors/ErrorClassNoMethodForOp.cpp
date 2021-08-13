//
// Created by chris on 12/8/21.
//

#include "ErrorClassNoMethodForOp.h"

Error* ErrorClassNoMethodForOp::clone() const {
    return nullptr;
}

bool ErrorClassNoMethodForOp::equal(const Error& other) const {
    return false;
}

std::string ErrorClassNoMethodForOp::to_str() const {
    return std::string();
}

ErrorClassNoMethodForOp::ErrorClassNoMethodForOp(std::string class_name, std::string op, const Node& node) {

}
