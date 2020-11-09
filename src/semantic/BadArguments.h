//
// Created by chris on 2/8/20.
//

#ifndef BADARGUMENTS_H
#define BADARGUMENTS_H

#include <stdexcept>
#include "../utils.h"

class BadArguments : public std::runtime_error {
public:
    BadArguments(VectorOfTypes ftype, VectorOfTypes args);

    bool operator==(const BadArguments& other) const;

    VectorOfTypes ftype;
    VectorOfTypes args;
    std::string text;

    const char* what() const noexcept override {

        return text.c_str();
//        return /* something using base_msg */;
    }
};


#endif //BADARGUMENTS_H
