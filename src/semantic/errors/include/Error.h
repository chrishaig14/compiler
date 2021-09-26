//
// Created by chris on 8/8/21.
//

#ifndef XLANG_ERROR_H
#define XLANG_ERROR_H


#include <string>
#include "../../../units/entities/Entity.h"
#include "../../../units/infos/Package.h"
#include "../../../units/infos/Enum.h"
#include "../../../logging/logging.h"



std::string entity_to_string(const Entity& entity);


class Error {
public:
    virtual ~Error() = default;
    virtual Error* clone() const = 0;
    virtual bool equal(const Error& other) const = 0;

    bool operator==(const Error& other) const;

    virtual std::string to_str(const CodeLines& code) const = 0;


};


#endif //XLANG_ERROR_H
