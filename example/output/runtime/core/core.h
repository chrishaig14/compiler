//
// Created by chris on 5/12/20.
//

#ifndef UNTITLED1_CORE_H
#define UNTITLED1_CORE_H

#include <iostream>
#include <map>
#include <assert.h>
#include "../Object.h"
#include "../BooleanObject.h"
#include "../IntegerObject.h"
#include "../ListObject.h"
#include "../StringObject.h"
#include "functions.h"
#include "GC.h"


XObject* f_print(XObject* _s);
#define CALL(fn, ...) (*fn)(__VA_ARGS__)
#define NEW(cls, ...) GC::register_object((TAG(new cls(__VA_ARGS__))))
#define CALL0(fn, ...) CALL(CAST(fn, Function0))
#define CALL1(fn, ...) CALL(CAST(fn, Function1),__VA_ARGS__)
#define CALL2(fn, ...) CALL(CAST(fn, Function2),__VA_ARGS__)
#define CALL3(fn, ...) CALL(CAST(fn, Function3),__VA_ARGS__)
#define INT_ADD(a, b) INT_TO_PTR(PTR_TO_INT(a) + PTR_TO_INT(b))
#define INT_SUB(a, b) INT_TO_PTR(PTR_TO_INT(a) - PTR_TO_INT(b))
#define INT_MUL(a, b) INT_TO_PTR(PTR_TO_INT(a) * PTR_TO_INT(b))
#define INT_DIV(a, b) INT_TO_PTR(PTR_TO_INT(a) / PTR_TO_INT(b))

#define LIST_SUBSCRIPT(list, index) CALL2(function_list_subscript, list, index)
#define LIST(...) NEW(XList, {__VA_ARGS__})
#define TUPLE2(...) NEW(Tuple2, __VA_ARGS__)
#define TUPLE3(...) NEW(Tuple3, __VA_ARGS__)
#define TUPLE4(...) NEW(Tuple4, __VA_ARGS__)
#define SET(x) GC::set("\"" #x "\"", x)
#define DECLARE(x) GC::declare("\"" #x "\"", x)
#define ENTER() GC::enter_local_scope()
#define LEAVE() GC::leave_local_scope()
#define ENTER_FUN(x) GC::enter_function(#x)
#define RETURN(x) return GC::function_return(x)
#define TEMP(x) GC::temp(x)

XObject* f_range(XObject* _start, XObject* _step, XObject* _end);

XObject* f_Integer_str(XObject* _i);

XObject* f_Boolean_str(XObject* _i);

XObject* f_map(XObject* _l, XObject* _f);

XObject* f_join(XObject* _l, XObject* _s);

XObject* f_List_add(XObject* _a, XObject* _b);
XObject* f_List_len(XObject* _l);

XObject* op_lt(XObject* a, XObject* b);

XObject* op_gt(XObject* a, XObject* b);

XObject* op_eq(XObject* a, XObject* b);
XObject* op_neq(XObject* a, XObject* b);

XObject* op_geq(XObject* a, XObject* b);

XObject* op_leq(XObject* a, XObject* b);


XObject* subscript(XObject* _l, XObject* i);


XObject* f_String_add(XObject* _a, XObject* _b);


class Tuple : public XObject {
public:

    std::vector<XObject*> members;

    Tuple(const std::string& n, int num);

    virtual XObject* get_member(int i) = 0;

    virtual void mark(std::vector<XObject*>& new_root) override;
};

class Tuple2 : public Tuple {
public:

    Tuple2(XObject* mem_1, XObject* mem_2);
    XObject* get_member(int i) override;
};

class Tuple3 : public Tuple {
public:

    Tuple3(XObject* mem_1, XObject* mem_2, XObject* mem_3);

    XObject* get_member(int i) override;
};

extern Function1 function_open_p;
extern Function1 function_print_p;
extern Function3 function_range_p;
extern Function2 function_map_p;
extern Function2 function_join_p;
extern Function1 function_File_read_line_p;
extern Function1 function_Integer_str_p;
extern Function1 function_Boolean_str_p;
extern Function2 function_List_add_p;
extern Function1 function_List_len_p;
extern Function2 function_list_subscript_p;
extern Function2 function_String_add_p;
extern Function1* function_open;
extern Function1* function_print;
extern Function3* function_range;
extern Function2* function_map;
extern Function2* function_join;
extern Function1* function_File_read_line;
extern Function1* function_Integer_str;
extern Function1* function_Boolean_str;
extern Function2* function_List_add;
extern Function1* function_List_len;
extern Function2* function_list_subscript;
extern Function2* function_dict_subscript;
extern Function2* function_string_subscript;
extern Function2* function_String_add;
extern Function1* function_String_len;


#endif //UNTITLED1_CORE_H
