//
// Created by chris on 2/8/20.
//

#include "Class.h"
#include "Entity.h"

bool Class::operator!=(const Class& b) const {
    return !(*this == b);
}

bool Class::operator==(const Class& b) const {
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

Class::~Class() {
    std::cout << "CALLING CLASS DESTRUCTOR!" << std::endl;
    for (const auto& m: this->members) {
        delete m.second;
    }
    // for (const auto& m: this->methods) {
    //     delete m.second;
    // }
    for (auto* m: this->member_types) {
        delete m;
    }
}

