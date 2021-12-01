//
// Created by chris on 20/11/21.
//

#ifndef XLANG_INSTANCEOBJECT_H
#define XLANG_INSTANCEOBJECT_H

#include <common/Path.h>
#include "Exp.h"

class sem::InstanceObject : public sem::Exp {
public:
    InstanceObject(const Path& class_path, Path typeclass_path);
    bool equals(const Exp& o) const override;
    UExp clone() const override;
    Path typeclass_path;
    Path class_path;
};


#endif //XLANG_INSTANCEOBJECT_H
