//
// Created by chris on 2/8/20.
//

#ifndef SCOPEERROR_H
#define SCOPEERROR_H

#include <stdexcept>

class ScopeError : public std::runtime_error {
public:
    ScopeError(std::string name);

    bool operator==(const ScopeError& other) const;
};

#endif //SCOPEERROR_H
