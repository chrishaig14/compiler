//
// Created by chris on 2/8/20.
//

#ifndef RETURNERROR_H
#define RETURNERROR_H

#include <stdexcept>
#include "../nodes/TypeNode.h"

class ReturnError : public std::runtime_error {
public:

    bool operator==(const ReturnError& other) const {
        std::string a = this->what();
        std::string b = other.what();
        bool t = a == b;
        return t;
    }

    ReturnError(TypeNode& expected_type, TypeNode& actual_type) : runtime_error(
            "Expected to return " + expected_type.to_string() + ", actual: " +
                    ((actual_type.kind == Kind::OBJECT && actual_type.object().identifier == ".None") ? "NULL"
                                                                                           : actual_type.to_string())
    ) {

    }

};

#endif //RETURNERROR_H
