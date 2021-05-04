//
// Created by chris on 25/4/21.
//

#include "Integer.h"
#include "../core.h"

DEFINE_FUNCTION(2, core_D_Integer_D_lt)
DEFINE_FUNCTION(2, core_D_Integer_D_gt)
DEFINE_FUNCTION(2, core_D_Integer_D_le)
DEFINE_FUNCTION(2, core_D_Integer_D_ge)
DEFINE_FUNCTION(2, core_D_Integer_D_eq)
DEFINE_FUNCTION(2, core_D_Integer_D_ne)
DEFINE_FUNCTION(2, core_D_Integer_D_add)
DEFINE_FUNCTION(2, core_D_Integer_D_sub)
DEFINE_FUNCTION(2, core_D_Integer_D_div)
DEFINE_FUNCTION(2, core_D_Integer_D_mul)
DEFINE_FUNCTION(1, core_D_Integer_D_str)

TaggedObject* core_D_Integer_D_lt_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) < GET_INT(b));
}

TaggedObject* core_D_Integer_D_gt_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) > GET_INT(b));
}

TaggedObject* core_D_Integer_D_le_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) <= GET_INT(b));
}

TaggedObject* core_D_Integer_D_ge_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) >= GET_INT(b));
}

TaggedObject* core_D_Integer_D_ne_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) != GET_INT(b));
}

TaggedObject* core_D_Integer_D_eq_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) == GET_INT(b));
}


TaggedObject* core_D_Integer_D_add_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) + GET_INT(b));
}

TaggedObject* core_D_Integer_D_sub_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) - GET_INT(b));
}

TaggedObject* core_D_Integer_D_div_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) / GET_INT(b));
}

TaggedObject* core_D_Integer_D_mul_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) * GET_INT(b));
}

TaggedObject* core_D_Integer_D_str_f(TaggedObject* _i) {
    TaggedObject* x = NEW(XString, std::to_string((GET_INT(_i))));
    return x;
}