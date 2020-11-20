//
// Created by chris on 16/8/20.
//

#include "AssignmentTypeError.h"

AssignmentTypeError::AssignmentTypeError(const TypeNode& expected_type, const TypeNode& actual_type)
        : std::runtime_error(
        "Assigning value of type " + actual_type.to_string() + ", expected: " + expected_type.to_string()) {
}

bool AssignmentTypeError::operator==(const AssignmentTypeError& other) const {
    std::string a = this->what();
    std::string b = other.what();
    bool t = a == b;
    return t;
}
