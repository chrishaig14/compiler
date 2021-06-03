//
// Created by chris on 3/3/21.
//

#include "XString.h"
#include "../basics.h"

TaggedObject* f_String_add(TaggedObject* _a, TaggedObject* _b) {
    GC::declare(_a);
    GC::declare(_b);
    XString* a = (XString*) UNTAG(_a);
    XString* b = (XString*) UNTAG(_b);
    auto r = MAKE_STRING(a->s + b->s);
    GC::out_of_scope(_a);
    GC::out_of_scope(_b);
    return r;
}

TaggedObject* f_String_len(TaggedObject* _a) {
    GC::declare(_a);
    XString* a = (XString*) UNTAG(_a);
    TaggedObject* r = MAKE_INT(a->s.size());
    GC::out_of_scope(_a);
    return r;
}

TaggedObject* f_String_eq(TaggedObject* this_obj, TaggedObject* other) {
    GC::declare(other);
    GC::declare(this_obj);
    TaggedObject* r = MAKE_BOOL(CAST(other, XString)->s == CAST(this_obj, XString)->s);
    GC::out_of_scope(other);
    GC::out_of_scope(this_obj);
    return r;
}

TaggedObject* f_String_ne(TaggedObject* this_obj, TaggedObject* other) {
    return MAKE_BOOL(not GET_BOOL(f_String_eq(this_obj, other)));
}


Function2 function_String_add_p = Function2(f_String_add);
Function1 function_String_len_p = Function1(f_String_len);
TaggedObject* function_String_add = FTAG(&function_String_add_p);
TaggedObject* function_String_len = FTAG(&function_String_len_p);
Function2 function_String_eq_p = Function2(f_String_eq);
Function2 function_String_ne_p = Function2(f_String_ne);
TaggedObject* function_String_eq = FTAG(&function_String_eq_p);

XString::~XString() {
    // std::cout << "Deleted String '" << this->s << "' (" << this << ")" << std::endl;
}

XString::XString(const std::string& x) : XObject("String") {
    this->s = x;
    this->is_string = true;
}


TaggedObject* XString::__eq__(TaggedObject* other) {
    return f_String_eq(TAG(this), other);
}

TaggedObject* core_D_core_D_String_D_len_f(TaggedObject* a) {
    return MAKE_INT(CAST(a, XString)->s.size());
}

TaggedObject* core_D_core_D_String_D___eq___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(CAST(a, XString)->s == CAST(b, XString)->s);
}


TaggedObject* core_D_core_D_String_D___ne___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(CAST(a, XString)->s != CAST(b, XString)->s);
}

DEFINE_FUNCTION(2, core_D_core_D_String_D___add__)
DEFINE_FUNCTION(1, core_D_core_D_String_D_len)

DEFINE_FUNCTION(2, core_D_core_D_String_D___eq__)
DEFINE_FUNCTION(2, core_D_core_D_String_D___ne__)

TaggedObject* core_D_core_D_String_D___add___f(TaggedObject* a, TaggedObject* b) {
    return MAKE_STRING(CAST(a, XString)->s + CAST(b, XString)->s);
}

DEFINE_FUNCTION(2, core_D_core_D_String_D___get_item__)

TaggedObject* core_D_core_D_String_D___get_item___f(TaggedObject* a, TaggedObject* b) {
    long index = GET_INT(b);
    unsigned long str_len = CAST(a, XString)->s.size();
    if (index >= str_len) {
        throw std::runtime_error(
                "String index out of range: " + std::to_string(index) + " but length is " + std::to_string(str_len));
    }

    return MAKE_STRING(std::string(1, CAST(a, XString)->s[index]));
}

DEFINE_FUNCTION(2, core_D_core_D_String_D_find_first)
TaggedObject* core_D_core_D_String_D_find_first_f(TaggedObject* a, TaggedObject* p) {
    XString* s = CAST(a, XString);
    for (int i = 0; i < s->s.size(); i++) {
        if (s->s[i] == CAST(p, XString)->s[0]) {
            return MAKE_INT(i);
        }
    }
    return MAKE_INT(-1);
}

DEFINE_FUNCTION(3, core_D_core_D_String_D_slice)
TaggedObject* core_D_core_D_String_D_slice_f(TaggedObject* a, TaggedObject* st, TaggedObject* en) {
    XString* s = CAST(a, XString);
    std::string rs;
    for (int i = GET_INT(st); i < GET_INT(en); i++) {
        rs += s->s[i];
    }
    return NEW(XString, rs);
}