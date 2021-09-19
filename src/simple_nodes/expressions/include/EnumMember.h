//
// Created by chris on 1/5/21.
//

#ifndef XLANG_ENUMMEMBER_H
#define XLANG_ENUMMEMBER_H


#include <string>
#include "Exp.h"

class sem::EnumMember : public sem::Exp {
public:
    EnumMember(const std::string& enum_name, const std::string& value);
    bool equals(const Exp& o) const override;

    std::string enum_name;
    std::string value;
};


#endif //XLANG_ENUMMEMBER_H
