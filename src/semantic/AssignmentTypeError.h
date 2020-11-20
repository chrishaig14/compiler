//
// Created by chris on 16/8/20.
//

#ifndef ASSIGNMENTTYPEERROR_H
#define ASSIGNMENTTYPEERROR_H


#include <stdexcept>
#include "../nodes/TypeNode.h"

class AssignmentTypeError : public std::runtime_error {
public:

    bool operator==(const AssignmentTypeError& other) const;

    AssignmentTypeError(const TypeNode& expected_type, const TypeNode& actual_type);

};


#endif //ASSIGNMENTTYPEERROR_H
