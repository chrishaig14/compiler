//
// Created by chris on 18/9/21.
//

#ifndef XLANG_OBJECTCONSTRUCTOR_H
#define XLANG_OBJECTCONSTRUCTOR_H


#include "Exp.h"
#include "../../../ast/Path.h"

class sem::ObjectConstructor : public sem::Exp {
public:
    Path class_path;

    explicit ObjectConstructor(Path class_path);
    bool equals(const Exp& o) const override;
};


#endif //XLANG_OBJECTCONSTRUCTOR_H
