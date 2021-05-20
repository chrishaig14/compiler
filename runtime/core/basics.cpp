//
// Created by chris on 2/4/21.
//

#include "basics.h"

TaggedObject* MAKE_STRING(const std::string& s) {
    return NEW(XString, s);
}