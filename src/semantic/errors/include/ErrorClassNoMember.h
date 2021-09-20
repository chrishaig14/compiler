//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORCLASSNOMEMBER_H
#define XLANG_ERRORCLASSNOMEMBER_H


#include "Error.h"

class ErrorClassNoMember : public Error {
public:
    ErrorClassNoMember(const ast::ObjectType& type, const std::string basic_string_1, TextPosition position,
                       const ast::CommonNode& node, TextPosition position_1, TextPosition position_2);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_ERRORCLASSNOMEMBER_H
