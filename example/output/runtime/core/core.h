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


XObject* f_print(XObject* _s) {
    XString* s = PTR_TO_STRING(_s);
    std::cout << s->s << std::endl;
    return nullptr;
}


XObject* f_range(XObject* _start, XObject* _step, XObject* _end) {
    long start = PTR_TO_INT(_start);
    long step = PTR_TO_INT(_step);
    long end = PTR_TO_INT(_end);
    std::vector<XObject*> v((end - start) / step, nullptr);
    int k = 0;
    for (int i = start; i < end; i += step) {
        v[k] = INT_TO_PTR(i);
        k++;
    }
    return GC::register_object(TAG(new XList(v)));
}

XObject* f_Integer_str(XObject* _i) {
    XObject* x = new XString(std::to_string((PTR_TO_INT(_i))));
    GC::register_object(TAG(x));
    return x;
}

XObject* f_Boolean_str(XObject* _i) {
    XObject* x = new XString((PTR_TO_BOOL(_i) ? "true" : "false"));
    GC::register_object(TAG(x));
    return x;
}

XObject* f_map(XObject* _l, XObject* _f) {
    std::vector<XObject*>* l = (std::vector<XObject*>*) _l;
    Function1* f = (Function1*) (_f);
    std::vector<XObject*> r;
    for (int i = 0; i < l->size(); i++) {
        r.push_back((*f)((*l)[i]));
    }
    return new XList(r);
}

XObject* f_join(XObject* _l, XObject* _s) {
    std::vector<XObject*>* l = (std::vector<XObject*>*) _l;
    std::string s;
    std::string js = *(std::string*) (_s);
    for (int i = 0; i < l->size(); i++) {
        s += *(std::string*) ((*l)[i]) + js;
    }
    s = s.substr(0, s.size() - js.size());
    return new XString(s);
}


XObject* f_List_add(XObject* _a, XObject* _b) {
    XList* a = (XList*) _a;
    XList* b = (XList*) _b;
    XList* r = new XList({});
    r->l.insert(r->l.end(), a->l.begin(), a->l.end());
    r->l.insert(r->l.end(), b->l.begin(), b->l.end());
    return r;
}

XObject* f_List_len(XObject* _l) {
    XList* l = (XList*) (UNTAG(_l));
    return INT_TO_PTR(l->l.size());
}


XObject* op_add(XObject* a, XObject* b) {
    return INT_TO_PTR(PTR_TO_INT(a) + PTR_TO_INT(b));
}

XObject* op_sub(XObject* a, XObject* b) {
    return INT_TO_PTR(PTR_TO_INT(a) - PTR_TO_INT(b));
}

XObject* op_div(XObject* a, XObject* b) {
    return INT_TO_PTR(PTR_TO_INT(a) / PTR_TO_INT(b));
}

XObject* op_mul(XObject* a, XObject* b) {
    return INT_TO_PTR(PTR_TO_INT(a) * PTR_TO_INT(b));
}

XObject* op_lt(XObject* a, XObject* b) {
    return BOOL_TO_PTR(PTR_TO_INT(a) < PTR_TO_INT(b));
}


XObject* op_gt(XObject* a, XObject* b) {
    return BOOL_TO_PTR(PTR_TO_INT(a) > PTR_TO_INT(b));
}


XObject* op_eq(XObject* a, XObject* b) {
    return BOOL_TO_PTR(PTR_TO_INT(a) == PTR_TO_INT(b));
}

XObject* op_neq(XObject* a, XObject* b) {
    return BOOL_TO_PTR(PTR_TO_INT(a) != PTR_TO_INT(b));
}

XObject* op_geq(XObject* a, XObject* b) {
    return BOOL_TO_PTR(PTR_TO_INT(a) >= PTR_TO_INT(b));
}


XObject* op_leq(XObject* a, XObject* b) {
    return BOOL_TO_PTR(PTR_TO_INT(a) <= PTR_TO_INT(b));
}


XObject* subscript(XObject* _l, XObject* i) {
    XObject* p = UNTAG(_l);
    if (p->is_list) {
        XList* list = (XList*) UNTAG(_l);
        unsigned long index = PTR_TO_INT(i);
        return list->l[index];
    }
    XDict* dict = (XDict*) UNTAG(_l);
    return dict->l.at(i);
}


XObject* f_String_add(XObject* _a, XObject* _b) {
    XString* a = (XString*) UNTAG(_a);
    XString* b = (XString*) UNTAG(_b);
    return GC::register_object(TAG(new XString(a->s + b->s)));
}

static Function1 function_print_p = Function1(f_print);
static Function3 function_range_p = Function3(f_range);
static Function2 function_map_p = Function2(f_map);
static Function2 function_join_p = Function2(f_join);
static Function1 function_Integer_str_p = Function1(f_Integer_str);
static Function1 function_Boolean_str_p = Function1(f_Boolean_str);
static Function2 function_List_add_p = Function2(f_List_add);
static Function1 function_List_len_p = Function1(f_List_len);
static Function2 function_String_add_p = Function2(f_String_add);

static Function1* function_print = &function_print_p;
static Function3* function_range = &function_range_p;
static Function2* function_map = &function_map_p;
static Function2* function_join = &function_join_p;
static Function1* function_Integer_str = &function_Integer_str_p;
static Function1* function_Boolean_str = &function_Boolean_str_p;
static Function2* function_List_add = &function_List_add_p;
static Function1* function_List_len = &function_List_len_p;
static Function2* function_String_add = &function_String_add_p;

class Tuple : public XObject {
public:

    std::vector<XObject*> members;

    Tuple(const std::string& n, int num) : XObject(n), members(num, nullptr) {}

    virtual XObject* get_member(int i) = 0;

    virtual void mark(std::vector<XObject*>& new_root) override {
        for (auto& m: this->members) {
            if (has_tag(m, OBJECT_TAG)) {
                XObject* element = UNTAG(m);
                if (!element->is_reachable() && !element->inserted) {
                    new_root.push_back(element);
                    element->inserted = true;
                }
            }
        }
    }
};

class Tuple2 : public Tuple {
public:

    Tuple2(XObject* mem_1, XObject* mem_2) : Tuple("Tuple2", 2) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 2);
        return this->members[i - 1];
    }
};

class Tuple3 : public Tuple {
public:

    Tuple3(XObject* mem_1, XObject* mem_2, XObject* mem_3) : Tuple("Tuple3", 3) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 3);
        return this->members[i - 1];
    }
};

class Tuple4 : public Tuple {
public:
    Tuple4(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4) : Tuple("Tuple4", 4) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 4);
        return this->members[i - 1];
    }
};

class Tuple5 : public Tuple {
public:
    Tuple5(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5) : Tuple("Tuple5", 5) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 5);
        return this->members[i - 1];
    }
};

class Tuple6 : public Tuple {
public:
    Tuple6(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6)
            : Tuple("Tuple6", 6) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 6);
        return this->members[i - 1];
    }
};

class Tuple7 : public Tuple {
public:
    Tuple7(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6,
           XObject* mem_7)
            : Tuple("Tuple7", 7) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
        this->members[6] = mem_7;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 7);
        return this->members[i - 1];
    }
};

class Tuple8 : public Tuple {
public:
    Tuple8(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6,
           XObject* mem_7,
           XObject* mem_8)
            : Tuple("Tuple8", 8) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
        this->members[6] = mem_7;
        this->members[7] = mem_8;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 8);
        return this->members[i - 1];
    }
};

class Tuple9 : public Tuple {
public:
    Tuple9(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6,
           XObject* mem_7,
           XObject* mem_8, XObject* mem_9)
            : Tuple("Tuple9", 9) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
        this->members[6] = mem_7;
        this->members[7] = mem_8;
        this->members[8] = mem_9;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 9);
        return this->members[i - 1];
    }
};

class Tuple10 : public Tuple {
public:
    Tuple10(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6,
            XObject* mem_7, XObject* mem_8, XObject* mem_9, XObject* mem_10)
            : Tuple("Tuple10", 10) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
        this->members[6] = mem_7;
        this->members[7] = mem_8;
        this->members[8] = mem_9;
        this->members[9] = mem_10;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 10);
        return this->members[i - 1];
    }
};

class Tuple11 : public Tuple {
public:
    Tuple11(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6,
            XObject* mem_7, XObject* mem_8, XObject* mem_9, XObject* mem_10, XObject* mem_11)
            : Tuple("Tuple11", 11) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
        this->members[6] = mem_7;
        this->members[7] = mem_8;
        this->members[8] = mem_9;
        this->members[9] = mem_10;
        this->members[10] = mem_11;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 11);
        return this->members[i - 1];
    }
};

class Tuple12 : public Tuple {
public:
    Tuple12(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6,
            XObject* mem_7, XObject* mem_8, XObject* mem_9, XObject* mem_10, XObject* mem_11, XObject* mem_12)
            : Tuple("Tuple12", 12) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
        this->members[6] = mem_7;
        this->members[7] = mem_8;
        this->members[8] = mem_9;
        this->members[9] = mem_10;
        this->members[10] = mem_11;
        this->members[11] = mem_12;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 12);
        return this->members[i - 1];
    }
};

class Tuple13 : public Tuple {
public:
    Tuple13(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6,
            XObject* mem_7, XObject* mem_8, XObject* mem_9, XObject* mem_10, XObject* mem_11, XObject* mem_12,
            XObject* mem_13)
            : Tuple("Tuple13", 13) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
        this->members[6] = mem_7;
        this->members[7] = mem_8;
        this->members[8] = mem_9;
        this->members[9] = mem_10;
        this->members[10] = mem_11;
        this->members[11] = mem_12;
        this->members[12] = mem_13;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 13);
        return this->members[i - 1];
    }
};

class Tuple14 : public Tuple {
public:
    Tuple14(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6,
            XObject* mem_7, XObject* mem_8, XObject* mem_9, XObject* mem_10, XObject* mem_11, XObject* mem_12,
            XObject* mem_13, XObject* mem_14)
            : Tuple("Tuple14", 14) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
        this->members[6] = mem_7;
        this->members[7] = mem_8;
        this->members[8] = mem_9;
        this->members[9] = mem_10;
        this->members[10] = mem_11;
        this->members[11] = mem_12;
        this->members[12] = mem_13;
        this->members[13] = mem_14;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 14);
        return this->members[i - 1];
    }
};

class Tuple15 : public Tuple {
public:
    Tuple15(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6,
            XObject* mem_7, XObject* mem_8, XObject* mem_9, XObject* mem_10, XObject* mem_11, XObject* mem_12,
            XObject* mem_13, XObject* mem_14, XObject* mem_15)
            : Tuple("Tuple15", 15) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
        this->members[6] = mem_7;
        this->members[7] = mem_8;
        this->members[8] = mem_9;
        this->members[9] = mem_10;
        this->members[10] = mem_11;
        this->members[11] = mem_12;
        this->members[12] = mem_13;
        this->members[13] = mem_14;
        this->members[14] = mem_15;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 15);
        return this->members[i - 1];
    }
};

class Tuple16 : public Tuple {
public:
    Tuple16(XObject* mem_1, XObject* mem_2, XObject* mem_3, XObject* mem_4, XObject* mem_5, XObject* mem_6,
            XObject* mem_7, XObject* mem_8, XObject* mem_9, XObject* mem_10, XObject* mem_11, XObject* mem_12,
            XObject* mem_13, XObject* mem_14, XObject* mem_15, XObject* mem_16)
            : Tuple("Tuple16", 16) {
        this->members[0] = mem_1;
        this->members[1] = mem_2;
        this->members[2] = mem_3;
        this->members[3] = mem_4;
        this->members[4] = mem_5;
        this->members[5] = mem_6;
        this->members[6] = mem_7;
        this->members[7] = mem_8;
        this->members[8] = mem_9;
        this->members[9] = mem_10;
        this->members[10] = mem_11;
        this->members[11] = mem_12;
        this->members[12] = mem_13;
        this->members[13] = mem_14;
        this->members[14] = mem_15;
        this->members[15] = mem_16;
    }

    XObject* get_member(int i) override {
        assert(i >= 1 && i <= 16);
        return this->members[i - 1];
    }
};

#endif //UNTITLED1_CORE_H