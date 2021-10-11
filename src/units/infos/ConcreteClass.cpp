//
// Created by chris on 2/8/20.
//

#include "ConcreteClass.h"
#include "../entities/Entity.h"

bool ConcreteClass::operator!=(const ConcreteClass& b) const {
    return !(*this == b);
}

bool ConcreteClass::operator==(const ConcreteClass& b) const {
    if (this->member_names != b.member_names) {
        return false;
    }
    if (this->member_types.size() != b.member_types.size()) {
        return false;
    }

    for (size_t i = 0; i < this->member_types.size(); i++) {
        if (this->member_types[i] != b.member_types[i]) {
            return false;
        }
    }
    return true;
}

ConcreteClass::~ConcreteClass() {
    for (const auto& m: this->members) {
        delete m.second;
    }
    for (auto* m: this->member_types) {
        delete m;
    }
}

ConcreteClass::ConcreteClass(const std::string& class_name, Path path) : path(path), class_name(class_name) {
}

