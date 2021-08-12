//
// Created by chris on 11/8/21.
//

#ifndef XLANG_ERRORNOMEMBER_H
#define XLANG_ERRORNOMEMBER_H


#include "Error.h"
#include "../../nodes/MemberNode.h"

class ErrorNoMember : public Error {
    const MemberNode& m;
    const TypeNode& t;
public:
    ErrorNoMember(const TypeNode& t, const MemberNode& m);

    bool equal(const Error& other) const override;

    std::string to_str() const override;
    Error* clone() const override;
};


#endif //XLANG_ERRORNOMEMBER_H
