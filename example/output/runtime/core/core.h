//
// Created by chris on 5/12/20.
//

#ifndef UNTITLED1_CORE_H
#define UNTITLED1_CORE_H

#include <iostream>
#include <map>
#include <assert.h>
#include "xobjects/XString.h"
#include "xobjects/XList.h"
#include "xobjects/XDict.h"
#include "xobjects/XFile.h"
#include "xobjects/XObject.h"
#include "xobjects/XTuple.h"
#include "functions.h"
#include "GC.h"
#include "../macros.h"
#include "basics.h"

TaggedObject* core_D_print_f(TaggedObject* _s);

TaggedObject* core_D_range_f(TaggedObject* _start, TaggedObject* _step, TaggedObject* _end);

TaggedObject* core_D_Integer_D_str_f(TaggedObject* _i);

TaggedObject* core_D_Boolean_str_f(TaggedObject* _i);

TaggedObject* core_D_map_f(TaggedObject* _l, TaggedObject* _f);

TaggedObject* core_D_join_f(TaggedObject* _l, TaggedObject* _s);

TaggedObject* op_lt(TaggedObject* a, TaggedObject* b);

TaggedObject* op_gt(TaggedObject* a, TaggedObject* b);

TaggedObject* op_eq(TaggedObject* a, TaggedObject* b);

TaggedObject* op_neq(TaggedObject* a, TaggedObject* b);

TaggedObject* op_geq(TaggedObject* a, TaggedObject* b);

TaggedObject* op_leq(TaggedObject* a, TaggedObject* b);

TaggedObject* subscript(TaggedObject* _l, TaggedObject* i);

TaggedObject* core_D_Integer_D_add_f(TaggedObject* a, TaggedObject* b);

TaggedObject* core_D_Integer_D_sub_f(TaggedObject* a, TaggedObject* b);

TaggedObject* core_D_Integer_D_div_f(TaggedObject* a, TaggedObject* b);

TaggedObject* core_D_Integer_D_mul_f(TaggedObject* a, TaggedObject* b);

extern Function1 core_D_open_o;
extern Function1 core_D_orint_o;
extern Function3 core_D_range_o;
extern Function2 core_D_map_o;
extern Function2 core_D_join_o;
extern Function1 m_core_c_Double_f_str_o;
extern Function1 m_core_c_Float_f_str_o;
extern Function1 core_D_Boolean_D_str_o;
extern Function2 core_D_list_subscript_o;
extern Function1 core_D_Integer_D_str_o;
extern Function2 core_D_Integer_D_add_o;
extern Function2 core_D_Integer_D_sub_o;
extern Function2 core_D_Integer_D_div_o;
extern Function2 core_D_Integer_D_mul_o;
extern TaggedObject* core_D_open;
extern TaggedObject* core_D_print;
extern TaggedObject* core_D_range;
extern TaggedObject* core_D_map;
extern TaggedObject* core_D_join;
extern TaggedObject* core_D_Integer_D_str;
extern TaggedObject* core_D_Integer_D_add;
extern TaggedObject* core_D_Integer_D_sub;
extern TaggedObject* core_D_Integer_D_mul;
extern TaggedObject* core_D_Integer_D_div;
extern TaggedObject* m_core_c_Float_f_str;
extern TaggedObject* m_core_c_Double_f_str;
extern TaggedObject* m_core_c_Boolean_f_str;
extern TaggedObject* m_core_list_subscript;
extern TaggedObject* m_core_dict_subscript;
extern TaggedObject* m_core_string_subscript;

#endif //UNTITLED1_CORE_H
