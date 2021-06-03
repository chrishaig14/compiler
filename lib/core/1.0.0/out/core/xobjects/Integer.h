//
// Created by chris on 25/4/21.
//

#ifndef XLANG_INTEGER_H
#define XLANG_INTEGER_H

#include "../Function.h"

extern Function1 core_D_core_D_Integer_D_str_o;

extern Function2 core_D_core_D_Integer_D___add___o;
extern Function2 core_D_core_D_Integer_D___div___o;
extern Function2 core_D_core_D_Integer_D___mod___o;
extern Function2 core_D_core_D_Integer_D___mul___o;
extern Function2 core_D_core_D_Integer_D___sub___o;

extern Function2 core_D_core_D_Integer_D___lt___o;
extern Function2 core_D_core_D_Integer_D___gt___o;
extern Function2 core_D_core_D_Integer_D___le___o;
extern Function2 core_D_core_D_Integer_D___ge___o;
extern Function2 core_D_core_D_Integer_D___eq___o;
extern Function2 core_D_core_D_Integer_D___ne___o;

extern Function1 core_D_core_D_Integer_D_parse_o;

extern TaggedObject* core_D_core_D_Integer_D_str;

extern TaggedObject* core_D_core_D_Integer_D___lt__;
extern TaggedObject* core_D_core_D_Integer_D___gt__;
extern TaggedObject* core_D_core_D_Integer_D___le__;
extern TaggedObject* core_D_core_D_Integer_D___ge__;
extern TaggedObject* core_D_core_D_Integer_D___eq__;
extern TaggedObject* core_D_core_D_Integer_D___ne__;

extern TaggedObject* core_D_core_D_Integer_D___add__;
extern TaggedObject* core_D_core_D_Integer_D___div__;
extern TaggedObject* core_D_core_D_Integer_D___mod__;
extern TaggedObject* core_D_core_D_Integer_D___mul__;
extern TaggedObject* core_D_core_D_Integer_D___sub__;

extern TaggedObject* core_D_core_D_Integer_D_parse;

TaggedObject* core_D_core_D_Integer_D_str_f(TaggedObject* _i);

TaggedObject* core_D_core_D_Integer_D___add___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_Integer_D___div___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_Integer_D___mod___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_Integer_D___mul___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_Integer_D___sub___f(TaggedObject* a, TaggedObject* b);

TaggedObject* core_D_core_D_Integer_D___lt___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_Integer_D___gt___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_Integer_D___le___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_Integer_D___ge___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_Integer_D___ne___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_Integer_D___eq___f(TaggedObject* a, TaggedObject* b);

TaggedObject* core_D_core_D_Integer_D_parse_f(TaggedObject* a);

#endif //XLANG_INTEGER_H
