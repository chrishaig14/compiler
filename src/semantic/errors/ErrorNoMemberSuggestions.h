//
// Created by chris on 11/8/21.
//

#ifndef XLANG_ERRORNOMEMBERSUGGESTIONS_H
#define XLANG_ERRORNOMEMBERSUGGESTIONS_H

#include "Error.h"
#include "../../ast/Member.h"

class ErrorNoMemberSuggestions : public Error {
    const ast::Member& m;
    const ast::Type& t;
    const Class& clazz;
public:
    ErrorNoMemberSuggestions(const ast::Type& t, const ast::Member& m, const Class& clazz);

    bool equal(const Error& other) const override;

    std::string to_str() const override;
    Error* clone() const override;
};


#endif //XLANG_ERRORNOMEMBERSUGGESTIONS_H
