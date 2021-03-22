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


Function2 function_String_add_p = Function2(f_String_add);
Function1 function_String_len_p = Function1(f_String_len);
TaggedObject* function_String_add = FTAG(&function_String_add_p);
TaggedObject* function_String_len = FTAG(&function_String_len_p);
Function2 function_String_eq_p = Function2(f_String_eq);
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
