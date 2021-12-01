//
// Created by chris on 11/4/21.
//

#include "../include/ObjectMember.h"

using namespace sem;

bool ObjectMember::equals(const Exp& o) const {
    auto& other = (const ObjectMember&) o;
    bool obj_ok = *this->object == *other.object;
    bool class_ok = this->class_path.as_str() == other.class_path.as_str();
    bool member_ok = this->member_name == other.member_name;
    return obj_ok && class_ok && member_ok;
}

ObjectMember::ObjectMember(UExp object, Path class_path, std::string member_name)
        : Exp(ExpType::OBJECT_MEMBER), object(std::move(object)), class_path(class_path), member_name(member_name) {
}

UExp ObjectMember::clone() const {
    return std::make_unique<ObjectMember>(*this);
}

ObjectMember::ObjectMember(const ObjectMember& other) : sem::Exp(ExpType::OBJECT_MEMBER) {
    this->object = other.object->clone();
    this->class_path = other.class_path;
    this->member_name = other.member_name;
}
