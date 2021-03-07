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

TaggedObject* f_print(TaggedObject* _s);

TaggedObject* f_range(TaggedObject* _start, TaggedObject* _step, TaggedObject* _end);

TaggedObject* f_Integer_str(TaggedObject* _i);

TaggedObject* f_Boolean_str(TaggedObject* _i);

TaggedObject* f_map(TaggedObject* _l, TaggedObject* _f);

TaggedObject* f_join(TaggedObject* _l, TaggedObject* _s);

TaggedObject* op_lt(TaggedObject* a, TaggedObject* b);

TaggedObject* op_gt(TaggedObject* a, TaggedObject* b);

TaggedObject* op_eq(TaggedObject* a, TaggedObject* b);

TaggedObject* op_neq(TaggedObject* a, TaggedObject* b);

TaggedObject* op_geq(TaggedObject* a, TaggedObject* b);

TaggedObject* op_leq(TaggedObject* a, TaggedObject* b);

TaggedObject* subscript(TaggedObject* _l, TaggedObject* i);

extern Function1 function_open_p;
extern Function1 function_print_p;
extern Function3 function_range_p;
extern Function2 function_map_p;
extern Function2 function_join_p;
extern Function1 function_Integer_str_p;
extern Function1 function_Boolean_str_p;
extern Function2 function_list_subscript_p;
extern TaggedObject* function_open;
extern TaggedObject* function_print;
extern TaggedObject* function_range;
extern TaggedObject* function_map;
extern TaggedObject* function_join;
extern TaggedObject* function_Integer_str;
extern TaggedObject* function_Boolean_str;
extern TaggedObject* function_list_subscript;
extern TaggedObject* function_dict_subscript;
extern TaggedObject* function_string_subscript;

#endif //UNTITLED1_CORE_H
