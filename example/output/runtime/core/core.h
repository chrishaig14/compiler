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

TaggedObject* m_core_f_print_f(TaggedObject* _s);

TaggedObject* m_core_f_range_f(TaggedObject* _start, TaggedObject* _step, TaggedObject* _end);

TaggedObject* m_core_f_Integer_str_f(TaggedObject* _i);

TaggedObject* m_core_f_Boolean_str_f(TaggedObject* _i);

TaggedObject* m_core_f_map_f(TaggedObject* _l, TaggedObject* _f);

TaggedObject* m_core_f_join_f(TaggedObject* _l, TaggedObject* _s);

TaggedObject* op_lt(TaggedObject* a, TaggedObject* b);

TaggedObject* op_gt(TaggedObject* a, TaggedObject* b);

TaggedObject* op_eq(TaggedObject* a, TaggedObject* b);

TaggedObject* op_neq(TaggedObject* a, TaggedObject* b);

TaggedObject* op_geq(TaggedObject* a, TaggedObject* b);

TaggedObject* op_leq(TaggedObject* a, TaggedObject* b);

TaggedObject* subscript(TaggedObject* _l, TaggedObject* i);

extern Function1 m_core_f_open_o;
extern Function1 m_core_f_orint_o;
extern Function3 m_core_f_range_o;
extern Function2 m_core_f_map_o;
extern Function2 m_core_f_join_o;
extern Function1 m_core_c_Integer_f_str_o;
extern Function1 m_core_c_Double_f_str_o;
extern Function1 m_core_c_Float_f_str_o;
extern Function1 m_core_c_Boolean_f_str_o;
extern Function2 m_core_list_subscript_o;
extern TaggedObject* m_core_f_open;
extern TaggedObject* m_core_f_print;
extern TaggedObject* m_core_f_range;
extern TaggedObject* m_core_f_map;
extern TaggedObject* m_core_f_join;
extern TaggedObject* m_core_c_Integer_f_str;
extern TaggedObject* m_core_c_Float_f_str;
extern TaggedObject* m_core_c_Double_f_str;
extern TaggedObject* m_core_c_Boolean_f_str;
extern TaggedObject* m_core_list_subscript;
extern TaggedObject* m_core_dict_subscript;
extern TaggedObject* m_core_string_subscript;

#endif //UNTITLED1_CORE_H
