//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XSTRING_H
#define XLANG_XSTRING_H

#include "XObject.h"
#include "../Function.h"
#include "../GC.h"

class XString : public XObject {
public:
    std::string s;

    XString(const std::string& x);

    ~XString();

    TaggedObject* __eq__(TaggedObject* other) override;
};

extern Function2 core_D_String_D_eq_o;
extern Function1 core_D_String_D_len_o;
extern Function2 core_D_String_D_add_o;
extern Function2 core_D_String_D___sub___o;

extern TaggedObject* core_D_String_D_eq;
extern TaggedObject* core_D_String_D_add;
extern TaggedObject* core_D_String_D_len;
extern TaggedObject* core_D_String_D___sub__;
extern TaggedObject* core_D_String_D_eq;

TaggedObject* core_D_String_D_add_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_String_D_eq_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_String_D_len_f(TaggedObject* a);
TaggedObject* core_D_String_D___sub___f(TaggedObject* a, TaggedObject* b);

#endif //XLANG_XSTRING_H
