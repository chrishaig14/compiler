//
// Created by chris on 25/4/21.
//

#include "Integer.h"
#include "../core.h"

DEFINE_FUNCTION(2, core_D_core_D_Integer_D___lt__)
DEFINE_FUNCTION(2, core_D_core_D_Integer_D___gt__)
DEFINE_FUNCTION(2, core_D_core_D_Integer_D___le__)
DEFINE_FUNCTION(2, core_D_core_D_Integer_D___ge__)
DEFINE_FUNCTION(2, core_D_core_D_Integer_D___eq__)
DEFINE_FUNCTION(2, core_D_core_D_Integer_D___ne__)
DEFINE_FUNCTION(2, core_D_core_D_Integer_D___add__)
DEFINE_FUNCTION(2, core_D_core_D_Integer_D___div__)
DEFINE_FUNCTION(2, core_D_core_D_Integer_D___mod__)
DEFINE_FUNCTION(2, core_D_core_D_Integer_D___mul__)
DEFINE_FUNCTION(2, core_D_core_D_Integer_D___sub__)
DEFINE_FUNCTION(1, core_D_core_D_Integer_D_str)

TaggedObject* core_D_core_D_Integer_D___lt___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) < GET_INT(b));
}

TaggedObject* core_D_core_D_Integer_D___gt___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) > GET_INT(b));
}

TaggedObject* core_D_core_D_Integer_D___le___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) <= GET_INT(b));
}

TaggedObject* core_D_core_D_Integer_D___ge___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) >= GET_INT(b));
}

TaggedObject* core_D_core_D_Integer_D___ne___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) != GET_INT(b));
}

TaggedObject* core_D_core_D_Integer_D___eq___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) == GET_INT(b));
}

TaggedObject* core_D_core_D_Integer_D___add___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) + GET_INT(b));
}

TaggedObject* core_D_core_D_Integer_D___div___f(TaggedObject* a, TaggedObject* b) {
    int b_int = GET_INT(b);
    if (b_int == 0) {
        throw std::runtime_error("Division by zero error!");
    }
    return MAKE_INT(GET_INT(a) / b_int);
}

TaggedObject* core_D_core_D_Integer_D___mod___f(TaggedObject* a, TaggedObject* b) {
    int b_int = GET_INT(b);
    if (b_int == 0) {
        throw std::runtime_error("Division by zero error!");
    }
    return MAKE_INT(GET_INT(a) % b_int);
}

TaggedObject* core_D_core_D_Integer_D___mul___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) * GET_INT(b));
}

TaggedObject* core_D_core_D_Integer_D___sub___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) - GET_INT(b));
}

TaggedObject* core_D_core_D_Integer_D_str_f(TaggedObject* _i) {
    TaggedObject* x = NEW(XString, std::to_string((GET_INT(_i))));
    return x;
}

DEFINE_FUNCTION(1, core_D_core_D_Integer_D_parse)

TaggedObject* core_D_core_D_Integer_D_parse_f(TaggedObject* _i) {
    return MAKE_INT(std::atoi(CAST(_i, XString)->s.c_str()));
}