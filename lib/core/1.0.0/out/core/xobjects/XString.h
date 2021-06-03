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
extern Function2 core_D_core_D_String_D___ne___o;
extern Function1 core_D_core_D_String_D_len_o;
extern Function2 core_D_core_D_String_D___add___o;
extern Function2 core_D_core_D_String_D___get_item___o;
extern Function2 core_D_core_D_String_D_find_first_o;
extern Function3 core_D_core_D_String_D_slice_o;

extern TaggedObject* core_D_core_D_String_D___eq__;
extern TaggedObject* core_D_core_D_String_D___ne__;
extern TaggedObject* core_D_core_D_String_D___add__;
extern TaggedObject* core_D_core_D_String_D_len;
extern TaggedObject* core_D_core_D_String_D___get_item__;
extern TaggedObject* core_D_core_D_String_D___eq__;
extern TaggedObject* core_D_core_D_String_D_find_first;
extern TaggedObject* core_D_core_D_String_D_slice;

TaggedObject* core_D_core_D_String_D___add___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_String_D___eq___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_String_D___ne___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_String_D_len_f(TaggedObject* a);
TaggedObject* core_D_core_D_String_D___get_item___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_String_D_find_first_f(TaggedObject* a, TaggedObject* p);
TaggedObject* core_D_core_D_String_D_slice_f(TaggedObject* a, TaggedObject* s, TaggedObject* e);



/*fun find_first(s: String, p: String) -> Integer {
    for i @ range(0,1,s.len()) {
        if s[i] == p {
            return i
        }
    }
    return 0-1
}

fun slice(s:String, start: Integer, end: Integer)->String {
    var result = ""
    for i @ range(0,1,s.len()) {
        if i >= start and i < end {
            result += s[i]
        }
    }
    return result
}
* */

#endif //XLANG_XSTRING_H
