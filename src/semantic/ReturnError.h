//
// Created by chris on 2/8/20.
//

#ifndef RETURNERROR_H
#define RETURNERROR_H

#include <stdexcept>
#include "../nodes/TypeNode.h"

class ReturnError : public std::runtime_error {
public:

    bool operator==(const ReturnError& other) const;

    ReturnError(const TypeNode& expected_type, const TypeNode& actual_type);

};

#endif //RETURNERROR_H
