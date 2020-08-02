//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_BADARGUMENTS_H
#define UNTITLED1_BADARGUMENTS_H

#include <stdexcept>

class BadArguments : public std::runtime_error {
public:
    BadArguments();

    bool operator==(const BadArguments& other) const;
};


#endif //UNTITLED1_BADARGUMENTS_H
