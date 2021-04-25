//
// Created by chris on 25/4/21.
//

#include "Boolean.h"
#include "../core.h"

DEFINE_FUNCTION(2, core_D_Boolean_D_or)
DEFINE_FUNCTION(2, core_D_Boolean_D_and)
DEFINE_FUNCTION(2, core_D_Boolean_D_eq)
DEFINE_FUNCTION(2, core_D_Boolean_D_ne)
DEFINE_FUNCTION(1, core_D_Boolean_D_str)

TaggedObject* core_D_Boolean_D_eq_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_BOOL(a) == GET_BOOL(b));
}

TaggedObject* core_D_Boolean_D_ne_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_BOOL(a) != GET_BOOL(b));
}

TaggedObject* core_D_Boolean_D_and_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_BOOL(a) && GET_BOOL(b));
}

TaggedObject* core_D_Boolean_D_or_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_BOOL(a) || GET_BOOL(b));
}

TaggedObject* core_D_Boolean_D_str_f(TaggedObject* _i) {
    TaggedObject* x = NEW(XString, ((GET_BOOL(_i) ? "true" : "false")));
    return x;
}