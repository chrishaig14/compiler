//
// Created by chris on 11/8/21.
//

#ifndef XLANG_ERRORNOMEMBER_H
#define XLANG_ERRORNOMEMBER_H


#include "Error.h"
#include "../../ast/Member.h"

class ErrorNoMember : public Error {
    const ast::Member& m;
    const ast::TypeNode& t;
public:
    ErrorNoMember(const ast::TypeNode& t, const ast::Member& m);

    bool equal(const Error& other) const override;

    std::string to_str() const override;
    Error* clone() const override;
};


#endif //XLANG_ERRORNOMEMBER_H
