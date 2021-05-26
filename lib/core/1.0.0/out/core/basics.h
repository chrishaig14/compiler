//
// Created by chris on 6/3/21.
//

#ifndef XLANG_BASICS_H
#define XLANG_BASICS_H

#include "xobject.h"
#include "xobjects/XString.h"
#include "macros.h"
#include "xobjects/XDouble.h"

TaggedObject* MAKE_STRING(const std::string& s);


inline TaggedObject* MAKE_DOUBLE(double d) {
    return NEW(XDouble, d);
}

inline TaggedObject* EQ(TaggedObject* a, TaggedObject* b) {
    if (a == b) {
        return MAKE_BOOL(true);
    }
    if (has_tag(a, OBJECT_TAG)) {
        return UNTAG(a)->__eq__(b);
    }
    return MAKE_BOOL(false);
}



inline TaggedObject* INT_ADD(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) + GET_INT(b));
}

inline TaggedObject* FLOAT_ADD(TaggedObject* a, TaggedObject* b) {
    return MAKE_FLOAT(GET_FLOAT(a) + GET_FLOAT(b));
}

inline TaggedObject* FLOAT_SUB(TaggedObject* a, TaggedObject* b) {
    return MAKE_FLOAT(GET_FLOAT(a) - GET_FLOAT(b));
}

inline TaggedObject* FLOAT_DIV(TaggedObject* a, TaggedObject* b) {
    return MAKE_FLOAT(GET_FLOAT(a) / GET_FLOAT(b));
}

inline TaggedObject* FLOAT_MUL(TaggedObject* a, TaggedObject* b) {
    return MAKE_FLOAT(GET_FLOAT(a) * GET_FLOAT(b));
}

inline TaggedObject* DOUBLE_ADD(TaggedObject* a, TaggedObject* b) {
    return MAKE_DOUBLE(CAST(a, XDouble)->d + CAST(b, XDouble)->d);
}

inline TaggedObject* DOUBLE_SUB(TaggedObject* a, TaggedObject* b) {
    return MAKE_DOUBLE(CAST(a, XDouble)->d - CAST(b, XDouble)->d);
}

inline TaggedObject* DOUBLE_DIV(TaggedObject* a, TaggedObject* b) {
    return MAKE_DOUBLE(CAST(a, XDouble)->d / CAST(b, XDouble)->d);
}

inline TaggedObject* DOUBLE_MUL(TaggedObject* a, TaggedObject* b) {
    return MAKE_DOUBLE(CAST(a, XDouble)->d * CAST(b, XDouble)->d);
}

inline TaggedObject* INT_SUB(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) - GET_INT(b));
}

inline TaggedObject* INT_MUL(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) * GET_INT(b));
}

inline TaggedObject* INT_MOD(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) % GET_INT(b));
}

inline TaggedObject* INT_DIV(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) / GET_INT(b));
}

inline TaggedObject* Integer_to_Float(TaggedObject* a) {
    return MAKE_FLOAT(GET_INT(a));
}
inline TaggedObject* Integer_to_Double(TaggedObject* a) {
    return MAKE_DOUBLE(GET_INT(a));
}
inline TaggedObject* Integer_to_Boolean(TaggedObject* a) {
    return MAKE_BOOL(GET_INT(a));
}

inline TaggedObject* Float_to_Integer(TaggedObject* a) {
    return MAKE_INT(GET_FLOAT(a));
}
inline TaggedObject* Float_to_Double(TaggedObject* a) {
    return MAKE_DOUBLE(GET_FLOAT(a));
}
inline TaggedObject* Float_to_Boolean(TaggedObject* a) {
    return MAKE_BOOL(GET_FLOAT(a));
}

inline TaggedObject* Double_to_Integer(TaggedObject* a) {
    return MAKE_INT(CAST(a,XDouble)->d);
}
inline TaggedObject* Double_to_Float(TaggedObject* a) {
    return MAKE_FLOAT(CAST(a,XDouble)->d);
}
inline TaggedObject* Double_to_Boolean(TaggedObject* a) {
    return MAKE_BOOL(CAST(a,XDouble)->d);
}

#endif //XLANG_BASICS_H
