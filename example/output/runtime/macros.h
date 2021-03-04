//
// Created by chris on 4/3/21.
//

#ifndef XLANG_MACROS_H
#define XLANG_MACROS_H

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
