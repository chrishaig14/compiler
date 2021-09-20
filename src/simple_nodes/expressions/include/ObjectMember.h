//
// Created by chris on 11/4/21.
//

#ifndef XLANG_OBJECTMEMBER_H
#define XLANG_OBJECTMEMBER_H


#include <string>
#include "Exp.h"
#include "../../../ast/general/Type.h"
#include "../../../util/Path.h"

class sem::ObjectMember : public sem::Exp {
public:
    UExp object;
    Path class_path;
    std::string member_name;

    ObjectMember(UExp object, Path class_path, std::string member_name);

    bool equals(const Exp& o) const override;

};


#endif //XLANG_OBJECTMEMBER_H
