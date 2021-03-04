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

XObject* f_print(XObject* _s);

XObject* f_range(XObject* _start, XObject* _step, XObject* _end);

XObject* f_Integer_str(XObject* _i);

XObject* f_Boolean_str(XObject* _i);

XObject* f_map(XObject* _l, XObject* _f);

XObject* f_join(XObject* _l, XObject* _s);

XObject* op_lt(XObject* a, XObject* b);

XObject* op_gt(XObject* a, XObject* b);

XObject* op_eq(XObject* a, XObject* b);

XObject* op_neq(XObject* a, XObject* b);

XObject* op_geq(XObject* a, XObject* b);

XObject* op_leq(XObject* a, XObject* b);

XObject* subscript(XObject* _l, XObject* i);

extern Function1 function_open_p;
extern Function1 function_print_p;
extern Function3 function_range_p;
extern Function2 function_map_p;
extern Function2 function_join_p;
extern Function1 function_Integer_str_p;
extern Function1 function_Boolean_str_p;
extern Function2 function_list_subscript_p;
extern Function1* function_open;
extern Function1* function_print;
extern Function3* function_range;
extern Function2* function_map;
extern Function2* function_join;
extern Function1* function_Integer_str;
extern Function1* function_Boolean_str;
extern Function2* function_list_subscript;
extern Function2* function_dict_subscript;
extern Function2* function_string_subscript;

#endif //UNTITLED1_CORE_H
