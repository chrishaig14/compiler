//
// Created by chris on 12/8/21.
//

#include "../include/ClassNoMember.h"

using namespace error;

Error* ClassNoMember::clone() const {
    return nullptr;
}

bool ClassNoMember::equal(const Error& other) const {
    return false;
}

std::string ClassNoMember::to_str(const CodeLines& code) const {
    return std::string();
}

ClassNoMember::ClassNoMember(Path class_path, const std::string& member_name, TextPosition position) : error::Error(ErrorType::class_no_member) {
    this->class_path = class_path;
    this->member_name = member_name;
    this->position = position;
}

