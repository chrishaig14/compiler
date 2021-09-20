//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORPACKAGENOMEMBER_H
#define XLANG_ERRORPACKAGENOMEMBER_H


#include "Error.h"

class ErrorPackageNoMember : public Error {
public:
    ErrorPackageNoMember(Package* p_package, const std::string basic_string_1, TextPosition position,
                         const ast::Statement& node, TextPosition position_1, TextPosition position_2);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_ERRORPACKAGENOMEMBER_H
