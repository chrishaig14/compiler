//
// Created by chris on 17/12/20.
//

#ifndef UNTITLED1_FUNCTIONS_H
#define UNTITLED1_FUNCTIONS_H

#include <vector>
#include "xobject.h"

#define FUNCTION_RAW_NAME(x) x##_f
#define FUNCTION_OBJECT_NAME(x) x##_o
#define FUNCTION_OBJECT(n, x) Function##n FUNCTION_OBJECT_NAME(x) = Function##n(FUNCTION_RAW_NAME(x));
#define DEFINE_FUNCTION(n, x) FUNCTION_OBJECT(n, x) TaggedObject* x = FTAG(&FUNCTION_OBJECT_NAME(x));


class Function0 : public XObject {
    TaggedObject* (* f)();
public:
    Function0();

    Function0(TaggedObject* (* f)());

    virtual TaggedObject* operator()();

};

class Function1 : public XObject {
    TaggedObject* (* f)(TaggedObject*);
public:
    Function1();

    Function1(TaggedObject* (* f)(TaggedObject*));

    virtual TaggedObject* operator()(TaggedObject* a0);

};

class Function2 : public XObject {
    TaggedObject* (* f)(TaggedObject*, TaggedObject*);
public:
    Function2();

    Function2(TaggedObject* (* f)(TaggedObject*, TaggedObject*));

    virtual TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1);

    ~Function2() override;
};

class Function3 : public XObject {
    TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*);
public:
    Function3();

    Function3(TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*));

    virtual TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1, TaggedObject* a2);

};

class Function4 : public XObject {
    TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*);
public:
    Function4();

    Function4(TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*));

    virtual TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1, TaggedObject* a2, TaggedObject* a3);

};

class Function5 : public XObject {
    TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*);
public:
    Function5();

    Function5(TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*));

    virtual TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1, TaggedObject* a2, TaggedObject* a3, TaggedObject* a4);

};


class Function6 : public XObject {
    TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*);
public:
    Function6();

    Function6(TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*));

    virtual TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1, TaggedObject* a2, TaggedObject* a3);

};


class Function7 : public XObject {
    TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*);
public:
    Function7();

    Function7(TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*));

    virtual TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1, TaggedObject* a2, TaggedObject* a3);

};

class Function8 : public XObject {
    TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*);
public:
    Function8();

    Function8(TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*));

    virtual TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1, TaggedObject* a2, TaggedObject* a3);

};

class Partial0 : public Function0 {
    std::vector<TaggedObject*> p;
    union {
        TaggedObject* f1;
        TaggedObject* f2;
        TaggedObject* f3;
    } f;
public:
    Partial0(TaggedObject* f, TaggedObject* p0);

    Partial0(TaggedObject* f, TaggedObject* p0, TaggedObject* p1);

    Partial0(TaggedObject* f, TaggedObject* p0, TaggedObject* p1, TaggedObject* p2);

    TaggedObject* operator()() override;
};

class Partial1 : public Function1 {
    std::vector<TaggedObject*> p;
    union {
        TaggedObject* f2;
        TaggedObject* f3;
    } f;
public:
    Partial1(TaggedObject* f, TaggedObject* p0, TaggedObject* p1);

    Partial1(TaggedObject* f, TaggedObject* p0, TaggedObject* p1, TaggedObject* p2);

    TaggedObject* operator()(TaggedObject* a0) override;
};

class Partial2 : public Function2 {
    std::vector<TaggedObject*> p;
    union {
        TaggedObject* f3;
    } f;
public:
    Partial2(TaggedObject* f, TaggedObject* p0, TaggedObject* p1, TaggedObject* p2);

    TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1) override;
};


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

inline TaggedObject*
CALL4(TaggedObject* f, TaggedObject* arg_0, TaggedObject* arg_1, TaggedObject* arg_2, TaggedObject* arg_4) {
    return (*CAST(f, Function4))(arg_0, arg_1, arg_2, arg_4);
}
inline TaggedObject*
CALL5(TaggedObject* f, TaggedObject* arg_0, TaggedObject* arg_1, TaggedObject* arg_2, TaggedObject* arg_4, TaggedObject* arg_5 ){
    return (*CAST(f, Function5))(arg_0, arg_1, arg_2, arg_4, arg_5);
}

#endif //UNTITLED1_FUNCTIONS_H
