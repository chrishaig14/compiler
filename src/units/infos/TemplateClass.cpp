//
// Created by chris on 2/8/20.
//

#include "TemplateClass.h"
#include "../entities/Entity.h"

bool TemplateClassInfo::operator!=(const TemplateClassInfo& b) const {
    return !(*this == b);
}

bool TemplateClassInfo::operator==(const TemplateClassInfo& b) const {
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

TemplateClassInfo::~TemplateClassInfo() {
    // for (const auto& m: this->members) {
    //     delete m.second;
    // }
    // for (auto* m: this->member_types) {
    //     delete m;
    // }
}

TemplateClassInfo::TemplateClassInfo(const std::string& class_name, Path path, VectorOfStrings type_params)
        : path(path), class_name(class_name), type_params(type_params) {
    assert(not type_params.empty());
}

