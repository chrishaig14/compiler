//
// Created by chris on 11/8/21.
//

#ifndef XLANG_NOMEMBERSUGGESTIONS_H
#define XLANG_NOMEMBERSUGGESTIONS_H

#include "Error.h"
#include <ast/expressions/include/Member.h>
#include <simple_nodes/common/include/TypeObject.h>

class error::NoMemberSuggestions : public error::Error {
    const ast::Member& m;
    sem::UType t;
    const ConcreteClass& clazz;
public:
    NoMemberSuggestions(const sem::Type& t, const ast::Member& m, const ConcreteClass& clazz);

    bool equal(const Error& other) const override;

    std::string to_str(const CodeLines& code) const override;
    Error* clone() const override;
};


#endif //XLANG_NOMEMBERSUGGESTIONS_H
