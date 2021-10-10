//
// Created by chris on 10/10/21.
//

#include "../include/StaticMethod.h"

sem::StaticMethod::StaticMethod(Path class_path, std::string method_name)
        : Exp(ExpType::STATIC_METHOD), class_path(class_path), method_name(method_name) {
}

bool sem::StaticMethod::equals(const sem::Exp& o) const {
    auto& other = (const StaticMethod&) o;
    bool class_ok = this->class_path.as_str() == other.class_path.as_str();
    bool member_ok = this->method_name == other.method_name;
    return class_ok && member_ok;
}
