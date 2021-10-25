//
// Created by chris on 25/10/21.
//

#ifndef XLANG_TYPECLASSNOTFOUND_H
#define XLANG_TYPECLASSNOTFOUND_H

#include "Error.h"
#include <ast/statements/Call.h>

class error::TypeclassNotFound : public error::Error {
public:
    const std::string& name;

    TypeclassNotFound(const std::string& name);

    Error* clone() const override;

    bool equal(const Error& other) const override;

    std::string to_str(const CodeLines& code) const override;

};

#endif //XLANG_TYPECLASSNOTFOUND_H
