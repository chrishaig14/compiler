//
// Created by chris on 30/4/21.
//

#ifndef XLANG_ENUM_H
#define XLANG_ENUM_H

#include <util/types.h>
#include "ConcreteClass.h"

class Enum {
public:
    const VectorOfStrings values;
    const std::string enumm_name;
    const Path path;

    Enum(std::string enumm_name, Path path, VectorOfStrings values);

    std::map<std::string, std::unique_ptr<ConstFunction>> functions;
};


#endif //XLANG_ENUM_H
