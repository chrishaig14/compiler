//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORNOTDECLARED_H
#define XLANG_ERRORNOTDECLARED_H


#include "Error.h"
#include "../../nodes/IdNode.h"

class ErrorNotDeclared : public Error {
    const IdNode& idn;
public:
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str() const override;

    explicit ErrorNotDeclared(const IdNode& idn);
};


#endif //XLANG_ERRORNOTDECLARED_H
