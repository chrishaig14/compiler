//
// Created by chris on 6/3/21.
//

#ifndef XLANG_BASICS_H
#define XLANG_BASICS_H

#include "xobject.h"
#include "xobjects/XString.h"
#include "../macros.h"

inline TaggedObject* MAKE_STRING(const std::string& s) {
    return NEW(XString, s);
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

inline TaggedObject* CALL0(TaggedObject* f) {
    return (*CAST(f, Function0))();
}

inline TaggedObject* CALL1(TaggedObject* f, TaggedObject* arg) {
    return (*CAST(f, Function1))(arg);
}

inline TaggedObject* CALL2(TaggedObject* f, TaggedObject* arg_0, TaggedObject* arg_1) {
    return (*CAST(f, Function2))(arg_0, arg_1);
}

inline TaggedObject* CALL3(TaggedObject* f, TaggedObject* arg_0, TaggedObject* arg_1, TaggedObject* arg_2) {
    return (*CAST(f, Function3))(arg_0, arg_1, arg_2);
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

inline TaggedObject* INT_SUB(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) - GET_INT(b));
}

inline TaggedObject* INT_MUL(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) * GET_INT(b));
}

inline TaggedObject* INT_DIV(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) / GET_INT(b));
}

#endif //XLANG_BASICS_H
