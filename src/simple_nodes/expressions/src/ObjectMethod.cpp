//
// Created by chris on 29/8/21.
//

#include "../include/ObjectMethod.h"

sem::ObjectMethod::ObjectMethod(sem::UExp object, Path class_path, std::string method_name) : Exp(ExpType::OBJECT_METHOD), object(std::move(object)), class_path(class_path),
                                                                                              method_name(method_name) {
}

bool sem::ObjectMethod::equals(const sem::Exp& o) const {
    auto& other = (const ObjectMethod&) o;
    bool obj_ok = *this->object == *other.object;
    bool class_ok = this->class_path.as_str() == other.class_path.as_str();
    bool member_ok = this->method_name == other.method_name;
    return obj_ok && class_ok && member_ok;
}
