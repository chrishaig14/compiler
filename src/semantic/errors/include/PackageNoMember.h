//
// Created by chris on 12/8/21.
//

#ifndef XLANG_PACKAGENOMEMBER_H
#define XLANG_PACKAGENOMEMBER_H


#include "Error.h"

class error::PackageNoMember : public error::Error {
public:
    PackageNoMember(Package* p_package, const std::string basic_string_1, TextPosition position,
                    const ast::Statement& node, TextPosition position_1, TextPosition position_2);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_PACKAGENOMEMBER_H
