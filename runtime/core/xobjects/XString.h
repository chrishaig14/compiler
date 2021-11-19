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

extern Function2 core_D_core_D_String_D___eq___o;
extern Function1 core_D_core_D_String_D_len_o;
extern Function2 core_D_core_D_String_D___add___o;
extern Function2 core_D_core_D_String_D___get_item___o;

extern TaggedObject* core_D_core_D_String_D___eq__;
extern TaggedObject* core_D_core_D_String_D___add__;
extern TaggedObject* core_D_core_D_String_D_len;
extern TaggedObject* core_D_core_D_String_D___get_item__;
extern TaggedObject* core_D_core_D_String_D___eq__;

TaggedObject* core_D_core_D_String_D___add___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_String_D___eq___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_String_D_len_f(TaggedObject* a);
TaggedObject* core_D_core_D_String_D___get_item___f(TaggedObject* a, TaggedObject* b);

#endif //XLANG_XSTRING_H
