//
// Created by chris on 8/8/21.
//

#ifndef XLANG_ERROR_H
#define XLANG_ERROR_H


#include <string>
#include "../../units/Entity.h"
#include "../../units/Package.h"
#include "../../units/Enum.h"
#include "../../units/ObjectValue.h"
#include "../../logging/logging.h"

std::string entity_to_string(const Entity& entity);


class Error {
public:
    virtual std::string to_str() = 0;
};


#endif //XLANG_ERROR_H
