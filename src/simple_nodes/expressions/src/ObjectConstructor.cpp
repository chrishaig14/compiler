//
// Created by chris on 18/9/21.
//

#include "../include/ObjectConstructor.h"

using namespace sem;

ObjectConstructor::ObjectConstructor(Path class_path) : Exp(ExpType::OBJECT_CONSTRUCTOR), class_path(class_path) {
}

bool ObjectConstructor::equals(const Exp& o) const {
    return this->class_path.as_str() == ((const ObjectConstructor&) o).class_path.as_str();
}
