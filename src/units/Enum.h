//
// Created by chris on 30/4/21.
//

#ifndef XLANG_ENUM_H
#define XLANG_ENUM_H

#include "../types.h"
#include "Class.h"

class Enum {
public:
    VectorOfStrings values;
    std::string enumm_name;
    Path path;
    std::map<std::string, ConstFunction*> functions;
};


#endif //XLANG_ENUM_H
