//
// Created by chris on 10/10/21.
//

#ifndef XLANG_STATICMETHOD_H
#define XLANG_STATICMETHOD_H

#include <string>
#include "Exp.h"
#include "../../../ast/general/Type.h"
#include "../../../util/Path.h"

class sem::StaticMethod : public sem::Exp {
public:
    Path class_path;
    std::string method_name;

    StaticMethod(Path class_path, std::string method_name);

    bool equals(const Exp& o) const override;

};

#endif //XLANG_STATICMETHOD_H
