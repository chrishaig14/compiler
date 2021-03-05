//
// Created by chris on 4/3/21.
//

#ifndef XLANG_MACROS_H
#define XLANG_MACROS_H

#define NEW(cls, ...) GC::register_object((TAG(new cls(__VA_ARGS__))))

inline TaggedObject* CALL0(TaggedObject* f) {
    return (*CAST(f, Function0))();
}

inline TaggedObject* CALL1(TaggedObject* f, TaggedObject* arg) {
    return (*CAST(f, Function1))(arg);
}

inline TaggedObject* CALL2(TaggedObject* f, TaggedObject* arg_0, TaggedObject* arg_1) {
    return (*CAST(f, Function2))(arg_0, arg_1);
}

inline TaggedObject* CALL3(TaggedObject* f, TaggedObject* arg_0, TaggedObject* arg_1, TaggedObject* arg_2) {
    return (*CAST(f, Function3))(arg_0, arg_1, arg_2);
}

inline TaggedObject* INT_ADD(TaggedObject* a, TaggedObject* b) {
    return INT_TO_PTR(PTR_TO_INT(a) + PTR_TO_INT(b));
}

inline TaggedObject* INT_SUB(TaggedObject* a, TaggedObject* b) {
    return INT_TO_PTR(PTR_TO_INT(a) - PTR_TO_INT(b));
}

inline TaggedObject* INT_MUL(TaggedObject* a, TaggedObject* b) {
    return INT_TO_PTR(PTR_TO_INT(a) * PTR_TO_INT(b));
}

inline TaggedObject* INT_DIV(TaggedObject* a, TaggedObject* b) {
    return INT_TO_PTR(PTR_TO_INT(a) / PTR_TO_INT(b));
}

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
