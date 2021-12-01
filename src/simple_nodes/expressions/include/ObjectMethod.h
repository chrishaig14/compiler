//
// Created by chris on 29/8/21.
//

#ifndef XLANG_OBJECTMETHOD_H
#define XLANG_OBJECTMETHOD_H

#include <string>
#include "Exp.h"
#include <ast/general/Type.h>
#include <common/Path.h>

class sem::ObjectMethod : public sem::Exp {
public:
    UExp object;
    Path class_path;
    std::string method_name;

    ObjectMethod(UExp object, Path class_path, std::string method_name);

    ObjectMethod(const ObjectMethod& other);

    bool equals(const Exp& o) const override;
    UExp clone() const override;

};


#endif //XLANG_OBJECTMETHOD_H
