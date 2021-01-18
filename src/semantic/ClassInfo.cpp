//
// Created by chris on 2/8/20.
//

#include "ClassInfo.h"

ClassInfo::ClassInfo(std::string class_name, const VectorOfStrings& fieldNames,
                     const VectorOfTypes& fieldTypes, VectorOfStrings type_parameters) : member_names(
        fieldNames), member_types(fieldTypes) {
    this->class_name = class_name;
    for (int i = 0; i < fieldNames.size(); i++) {
        this->members[fieldNames[i]] = fieldTypes[i];
    }
    for (int i = 0; i < fieldNames.size(); i++) {
        this->members[fieldNames[i]] = fieldTypes[i];
    }
    this->type_params = type_parameters;
}

bool ClassInfo::operator!=(const ClassInfo& b) const {
    return !(*this == b);
}

bool ClassInfo::operator==(const ClassInfo& b) const {
    if (this->member_names != b.member_names) {
        return false;
    }
    if (this->member_types.size() != b.member_types.size()) {
        return false;
    }

    for (int i = 0; i < this->member_types.size(); i++) {
        if (this->member_types[i] != b.member_types[i]) {
            return false;
        }
    }
    return true;
}
