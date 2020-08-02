//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_RETURNERROR_H
#define UNTITLED1_RETURNERROR_H

#include <stdexcept>

class ReturnError : public std::runtime_error {
public:

    bool operator==(const ReturnError& other) const {
        std::string a = this->what();
        std::string b = other.what();
        bool t = a == b;
        return t;
    }

    ReturnError(std::string actual_type, std::string expected_type) : runtime_error(
            "Expected to return " + expected_type + ", actual: " + actual_type) {

    }

};

#endif //UNTITLED1_RETURNERROR_H
