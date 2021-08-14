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
    virtual Error* clone() const = 0;
    virtual bool equal(const Error& other) const = 0;

    bool operator==(const Error& other) const;

    virtual std::string to_str() const = 0;


};

std::ostream& operator<<(std::ostream& o, const Error& e);


#endif //XLANG_ERROR_H
