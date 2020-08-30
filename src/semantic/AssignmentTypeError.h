//
// Created by chris on 16/8/20.
//

#ifndef UNTITLED1_ASSIGNMENTTYPEERROR_H
#define UNTITLED1_ASSIGNMENTTYPEERROR_H


#include <stdexcept>
#include "../nodes/TypeNode.h"

class AssignmentTypeError : public std::runtime_error {
public:

    bool operator==(const AssignmentTypeError& other) const {
        std::string a = this->what();
        std::string b = other.what();
        bool t = a == b;
        return t;
    }

    AssignmentTypeError(TypeNode* expected_type, TypeNode* actual_type) : std::runtime_error(
            "Assigning value of type " + actual_type->to_string() + ", expected: " + expected_type->to_string()) {
    }

};


#endif //UNTITLED1_ASSIGNMENTTYPEERROR_H
