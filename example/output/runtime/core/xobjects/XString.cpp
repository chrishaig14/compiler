//
// Created by chris on 3/3/21.
//

#include "XString.h"

XObject* f_String_add(XObject* _a, XObject* _b) {
    XString* a = (XString*) UNTAG(_a);
    XString* b = (XString*) UNTAG(_b);
    return GC::register_object(TAG(new XString(a->s + b->s)));
}

XObject* f_String_len(XObject* _a) {
    XString* a = (XString*) UNTAG(_a);
    return INT_TO_PTR(a->s.size());
}


Function2 function_String_add_p = Function2(f_String_add);
Function1 function_String_len_p = Function1(f_String_len);
Function2* function_String_add = &function_String_add_p;
Function1* function_String_len = &function_String_len_p;

XString::~XString() {
    // std::cout << "Deleted String '" << this->s << "' (" << this << ")" << std::endl;
}

XString::XString(const std::string& x) : XObject("String") {
    this->s = x;
    this->is_string = true;
}

void XString::mark(std::vector<XObject*>& x) {}
