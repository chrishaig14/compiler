//
// Created by chris on 12/8/21.
//

#ifndef XLANG_CLASSNOMEMBER_H
#define XLANG_CLASSNOMEMBER_H


#include "Error.h"

class error::ClassNoMember : public error::Error {
public:
    ClassNoMember(Path class_path, const std::string& member_name, TextPosition position);

    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    Path class_path;
    std::string member_name;
    TextPosition position;
};


#endif //XLANG_CLASSNOMEMBER_H
