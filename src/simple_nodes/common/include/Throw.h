//
// Created by chris on 19/6/21.
//

#ifndef XLANG_THROW_H
#define XLANG_THROW_H


#include "Common.h"
#include <vector>
#include <string>

class sem::Throw : public sem::Common {
public:
    Common* expression;
    std::vector<std::string> reachables;
    Throw(Common* expression);
};


#endif //XLANG_THROW_H
