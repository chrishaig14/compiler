//
// Created by chris on 25/4/21.
//

#ifndef XLANG_INTEGER_H
#define XLANG_INTEGER_H

#include "../Function.h"

extern Function1 core_D_Integer_D_str_o;
extern Function2 core_D_Integer_D_add_o;
extern Function2 core_D_Integer_D_sub_o;
extern Function2 core_D_Integer_D_div_o;
extern Function2 core_D_Integer_D_mul_o;
extern Function2 core_D_Integer_D_lt_o;
extern Function2 core_D_Integer_D_gt_o;
extern Function2 core_D_Integer_D_le_o;
extern Function2 core_D_Integer_D_ge_o;
extern Function2 core_D_Integer_D_eq_o;
extern Function2 core_D_Integer_D_ne_o;

extern TaggedObject* core_D_Integer_D_str;
extern TaggedObject* core_D_Integer_D_add;
extern TaggedObject* core_D_Integer_D_lt;
extern TaggedObject* core_D_Integer_D_gt;
extern TaggedObject* core_D_Integer_D_le;
extern TaggedObject* core_D_Integer_D_ge;
extern TaggedObject* core_D_Integer_D_eq;
extern TaggedObject* core_D_Integer_D_ne;
extern TaggedObject* core_D_Integer_D_sub;
extern TaggedObject* core_D_Integer_D_mul;
extern TaggedObject* core_D_Integer_D_div;

TaggedObject* core_D_Integer_D_add_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Integer_D_sub_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Integer_D_div_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Integer_D_mul_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Integer_D_lt_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Integer_D_gt_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Integer_D_le_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Integer_D_ge_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Integer_D_ne_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Integer_D_eq_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Integer_D_str_f(TaggedObject* _i);

#endif //XLANG_INTEGER_H
