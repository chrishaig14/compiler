//
// Created by chris on 1/5/21.
//

#ifndef XLANG_ENUMMEMBER_H
#define XLANG_ENUMMEMBER_H


#include <string>
#include "Exp.h"
#include <util/Path.h>

class sem::EnumMember : public sem::Exp {
public:
    EnumMember(Path enum_path, const std::string& value);
    bool equals(const Exp& o) const override;

    Path enum_path;
    std::string value;
};


#endif //XLANG_ENUMMEMBER_H
