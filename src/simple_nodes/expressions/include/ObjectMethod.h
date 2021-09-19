//
// Created by chris on 29/8/21.
//

#ifndef XLANG_OBJECTMETHOD_H
#define XLANG_OBJECTMETHOD_H

#include <string>
#include "../../expressions/include/Exp.h"
#include "../../../ast/Type.h"
#include "../../../ast/Path.h"

class sem::ObjectMethod : public sem::Exp {
public:
    UExp object;
    Path class_path;
    std::string method_name;

    ObjectMethod(UExp object, Path class_path, std::string method_name);

    bool equals(const Exp& o) const override;

};


#endif //XLANG_OBJECTMETHOD_H
