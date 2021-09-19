//
// Created by chris on 18/9/21.
//

#ifndef XLANG_OBJECTCONSTRUCTORCALL_H
#define XLANG_OBJECTCONSTRUCTORCALL_H


#include "../../expressions/include/Exp.h"
#include "../../../ast/Path.h"

class sem::ObjectConstructorCall : public sem::Exp {
public:
    Path class_path;
    std::vector<UExp> arguments;
    ObjectConstructorCall(Path class_path, std::vector<UExp> arguments);
    bool equals(const Exp& o) const override;
};


#endif //XLANG_OBJECTCONSTRUCTORCALL_H
