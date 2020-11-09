//
// Created by chris on 2/8/20.
//

#ifndef SCOPEERROR_H
#define SCOPEERROR_H

#include <stdexcept>

class ScopeError : public std::runtime_error {
public:
    ScopeError(std::string name) : runtime_error("Name '" + name + "' not found in current scope") {
    }

    bool operator==(const ScopeError& other) const {
        std::string a = this->what();
        std::string b = other.what();
        bool t = a == b;
        return t;
    }
};

#endif //SCOPEERROR_H
