//
// Created by chris on 20/11/21.
//

#include "InstanceObject.h"

bool sem::InstanceObject::equals(const sem::Exp& o) const {
    return false;
}

sem::InstanceObject::InstanceObject(const Path& class_path, Path typeclass_path)
        : Exp(ExpType::INSTANCE_OBJECT), typeclass_path(typeclass_path), class_path(class_path) {
}
