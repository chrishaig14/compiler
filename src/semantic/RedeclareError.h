//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_REDECLAREERROR_H
#define UNTITLED1_REDECLAREERROR_H

#include <string>
#include <stdexcept>

class RedeclareError : public std::runtime_error {
public:
    RedeclareError(std::string name);

    bool operator==(const RedeclareError& other) const;
};

#endif //UNTITLED1_REDECLAREERROR_H
