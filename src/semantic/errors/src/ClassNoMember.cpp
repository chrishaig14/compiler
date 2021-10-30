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

ClassNoMember::ClassNoMember(const ast::ObjectType& type, const std::string basic_string_1,
                             TextPosition position, const ast::Statement& node, TextPosition position_1,
                             TextPosition position_2):Error(ErrorType::class_no_member) {

}
