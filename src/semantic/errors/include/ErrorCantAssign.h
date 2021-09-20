//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORCANTASSIGN_H
#define XLANG_ERRORCANTASSIGN_H


#include "Error.h"

class ErrorCantAssign : public Error {
public:
    const ast::Statement& n;
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    ErrorCantAssign(const ast::Statement& n);

};


#endif //XLANG_ERRORCANTASSIGN_H
