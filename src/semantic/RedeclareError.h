//
// Created by chris on 2/8/20.
//

#ifndef REDECLAREERROR_H
#define REDECLAREERROR_H

#include <string>
#include <stdexcept>

class RedeclareError : public std::runtime_error {
public:
    RedeclareError(std::string name);

    bool operator==(const RedeclareError& other) const;
};

#endif //REDECLAREERROR_H
