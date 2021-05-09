//
// Created by chris on 25/4/21.
//

#include "Boolean.h"
#include "../core.h"

DEFINE_FUNCTION(2, core_D_Boolean_D___or__)
DEFINE_FUNCTION(2, core_D_Boolean_D___and__)
DEFINE_FUNCTION(2, core_D_Boolean_D___eq__)
DEFINE_FUNCTION(2, core_D_Boolean_D___ne__)
DEFINE_FUNCTION(1, core_D_Boolean_D_str)

TaggedObject* core_D_Boolean_D___eq___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_BOOL(a) == GET_BOOL(b));
}

TaggedObject* core_D_Boolean_D___ne___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_BOOL(a) != GET_BOOL(b));
}

TaggedObject* core_D_Boolean_D___and___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_BOOL(a) && GET_BOOL(b));
}

TaggedObject* core_D_Boolean_D___or___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_BOOL(a) || GET_BOOL(b));
}

TaggedObject* core_D_Boolean_D_str_f(TaggedObject* _i) {
    TaggedObject* x = NEW(XString, ((GET_BOOL(_i) ? "true" : "false")));
    return x;
}