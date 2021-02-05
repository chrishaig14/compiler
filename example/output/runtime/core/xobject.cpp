//
// Created by chris on 17/12/20.
//

#include "xobject.h"

XObject* hash(XObject* n) {
    XString* s = PTR_TO_STRING(n);
    const std::string& str = s->s;
    int h = 0;
    for (int i = 0; i < str.size(); i++) {
        h += int(str[i]);
    }
    return INT_TO_PTR(h);
}