//
// Created by chris on 11/8/21.
//

#ifndef XLANG_NOMEMBER_H
#define XLANG_NOMEMBER_H


#include "Error.h"
#include <ast/expressions/include/Member.h>

class error::NoMember : public error::Error {
    const ast::Member& m;
    const sem::Type& t;
public:
    NoMember(const sem::Type& t, const ast::Member& m);

    bool equal(const Error& other) const override;

    std::string to_str(const CodeLines& code) const override;
    Error* clone() const override;
};


#endif //XLANG_NOMEMBER_H
