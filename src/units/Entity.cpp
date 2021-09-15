//
// Created by chris on 6/4/21.
//

#include <string>
#include "Entity.h"

std::ostream& operator<<(std::ostream& o, ModuleMemberType f) {
    std::map<ModuleMemberType, std::string> str;
    str[ModuleMemberType::CONST_FUNCTION] = "const function";
    str[ModuleMemberType::CLASS] = "class";
    str[ModuleMemberType::ENUM] = "enum";
    str[ModuleMemberType::MODULE] = "module";
    str[ModuleMemberType::PACKAGE] = "package";
    return o << str[f];
}

std::string module_member_type_to_str(ModuleMemberType member_type) {
    std::string lt;
    switch (member_type) {
        case ModuleMemberType::CONST_FUNCTION:
            lt = "const function";
            break;
        case ModuleMemberType::CLASS:
            lt = "class";
            break;
        case ModuleMemberType::PACKAGE:
            lt = "package";
            break;
        case ModuleMemberType::MODULE:
            lt = "module";
            break;
        case ModuleMemberType::ENUM:
            lt = "enum";
            break;
    }
    return lt;
}