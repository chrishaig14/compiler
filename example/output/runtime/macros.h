//
// Created by chris on 4/3/21.
//

#ifndef XLANG_MACROS_H
#define XLANG_MACROS_H

#define NEW(cls, ...)(TAG(new cls(__VA_ARGS__)))
#define LIST_SUBSCRIPT(list, index) CALL2(function_list_subscript, list, index)
#define LIST(...) NEW(XList, {__VA_ARGS__})
#define TUPLE2(...) NEW(XTuple2, __VA_ARGS__)
#define TUPLE3(...) NEW(XTuple3, __VA_ARGS__)
#define TUPLE4(...) NEW(XTuple4, __VA_ARGS__)
#define SET(x) GC::set(#x, x)
#define DECLARE(x) GC::declare(#x, x)
#define ENTER() GC::enter_local_scope()
#define LEAVE() GC::leave_local_scope()
#define ENTER_FUN(x) GC::enter_function(#x)
#define RETURN(x) return GC::function_return(x)
#define TEMP(x) GC::temp(x)

#endif //XLANG_MACROS_H
