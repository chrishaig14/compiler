//
// Created by chris on 29/8/21.
//

#include "../include/ObjectMethod.h"

sem::ObjectMethod::ObjectMethod(sem::UExp object, Path class_path, std::string method_name)
        : Exp(ExpType::OBJECT_METHOD), object(std::move(object)), class_path(class_path), method_name(method_name) {
}

bool sem::ObjectMethod::equals(const sem::Exp& o) const {
    auto& other = (const ObjectMethod&) o;
    bool obj_ok = *this->object == *other.object;
    bool class_ok = this->class_path.as_str() == other.class_path.as_str();
    bool member_ok = this->method_name == other.method_name;
    return obj_ok && class_ok && member_ok;
}

sem::UExp sem::ObjectMethod::clone() const {
    return std::make_unique<ObjectMethod>(*this);
}

sem::ObjectMethod::ObjectMethod(const sem::ObjectMethod& other) : sem::Exp(ExpType::OBJECT_METHOD) {
    this->object = other.object->clone();
    this->class_path = other.class_path;
    this->method_name = other.method_name;
}
