//
// Created by chris on 2/8/20.
//

#include "ConcreteClass.h"
#include "../entities/Entity.h"

bool ConcreteClass::operator!=(const ConcreteClass& b) const {
    return !(*this == b);
}

bool ConcreteClass::operator==(const ConcreteClass& b) const {
    if (this->attribute_names != b.attribute_names) {
        return false;
    }
    if (this->attribute_types.size() != b.attribute_types.size()) {
        return false;
    }

    for (size_t i = 0; i < this->attribute_types.size(); i++) {
        if (this->attribute_types[i] != b.attribute_types[i]) {
            return false;
        }
    }
    return true;
}

ConcreteClass::~ConcreteClass() {
    // for (const auto& m: this->members) {
    //     delete m.second;
    // }
    // for (auto* m: this->member_types) {
    //     delete m;
    // }
}

ConcreteClass::ConcreteClass(const std::string& class_name, Path path) : path(path), class_name(class_name) {
}

