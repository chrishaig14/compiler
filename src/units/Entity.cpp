//
// Created by chris on 6/4/21.
//

#include <string>
#include "Entity.h"


std::string flirpintype_to_str(F_TYPE flirpintype) {
    std::string lt;
    switch (flirpintype) {
        case F_TYPE::CONST_FUNCTION:
            lt = "const function";
            break;
        case F_TYPE::CLASS:
            lt = "class";
            break;
        case F_TYPE::PACKAGE:
            lt = "package";
            break;
        case F_TYPE::MODULE:
            lt = "module";
            break;
        case F_TYPE::ENUM:
            lt = "enum";
            break;
    }
    return lt;
}