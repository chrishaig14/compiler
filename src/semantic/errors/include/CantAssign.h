//
// Created by chris on 12/8/21.
//

#ifndef XLANG_CANTASSIGN_H
#define XLANG_CANTASSIGN_H


#include "Error.h"

class error::CantAssign : public error::Error {
public:
    const ast::Statement& n;
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    CantAssign(const ast::Statement& n);

};


#endif //XLANG_CANTASSIGN_H
