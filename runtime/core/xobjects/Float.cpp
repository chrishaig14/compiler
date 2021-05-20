//
// Created by chris on 25/4/21.
//

#include <sstream>
#include <cstring>
#include "Float.h"

DEFINE_FUNCTION(1, core_D_Float_D_str)


TaggedObject* float_mk(float t) {
    TaggedObject* r = nullptr;
    memcpy((float*) &r + 1, &t, sizeof(float));
    r = (TaggedObject*) ((unsigned long) r | FLOAT_TAG);
    return r;
}

float float_rd(TaggedObject* r) {
    float e = 0;
    memcpy(&e, (float*) &r + 1, sizeof(float));
    return e;
}


TaggedObject* core_D_Float_D_str_f(TaggedObject* _i) {
    std::ostringstream out;
    out.precision(50);
    out << float_rd(_i);
    std::string s;
    s = out.str();
    TaggedObject* x = NEW(XString, s);
    return x;
}
