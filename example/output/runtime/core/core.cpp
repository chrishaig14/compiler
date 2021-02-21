//
// Created by chris on 5/12/20.
//

#include "core.h"

XObject* f_open(XObject* _s) {
    return new XFile(((XString*) UNTAG(_s))->s);
}

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
    return NEW(XList, v);
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
    XList* a = CAST(_a, XList);
    XList* b = CAST(_b, XList);
    XList* r = CAST(NEW(XList, std::vector<XObject*>()), XList);
    auto re = r->l.end();
    auto ab = a->l.begin();
    auto ae = a->l.end();
    r->l.insert(re, ab, ae);
    const std::vector<XObject*>::iterator& be = b->l.end();
    const std::vector<XObject*>::iterator& bb = b->l.begin();
    const std::vector<XObject*>::iterator& re2 = r->l.end();
    r->l.insert(re2, bb, be);
    return TAG(r);
}

XObject* f_List_len(XObject* _l) {
    XList* l = (XList*) (UNTAG(_l));
    return INT_TO_PTR(l->l.size());
}

XObject* op_lt(XObject* a, XObject* b) {
    return BOOL_TO_PTR(PTR_TO_INT(a) < PTR_TO_INT(b));
}


XObject* op_gt(XObject* a, XObject* b) {
    return BOOL_TO_PTR(PTR_TO_INT(a) > PTR_TO_INT(b));
}


XObject* op_eq(XObject* a, XObject* b) {
    if (has_tag(a, INT_TAG)) {
        return BOOL_TO_PTR(PTR_TO_INT(a) == PTR_TO_INT(b));
    }
    XObject* oa = UNTAG(a);
    XObject* ob = UNTAG(b);
    bool r = true;
    if (oa->is_string) {
        r = ((XString*) (oa))->s == ((XString*) (ob))->s;
    } else if (oa->is_list) {
        XList* la = (XList*) oa;
        XList* lb = (XList*) ob;
        if (la->l.size() != lb->l.size()) {
            r = false;
        } else {
            for (int i = 0; i < la->l.size(); i++) {
                if (!PTR_TO_BOOL(op_eq(la->l[i], lb->l[i]))) {
                    r = false;
                    break;
                }
            }
        }
    } else {
        Tuple* ta = (Tuple*) oa;
        Tuple* tb = (Tuple*) ob;
        for (int i = 0; i < ta->members.size(); i++) {
            if (!PTR_TO_BOOL(op_eq(ta->members[i], tb->members[i]))) {
                r = false;
                break;
            }
        }
    }
    return BOOL_TO_PTR(r);
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


XObject* dict_subscript(XObject* _l, XObject* i) {
    XDict* dict = (XDict*) UNTAG(_l);
    int h = hash(i);
    if (dict->l.find(h) == dict->l.end()) {
        throw std::runtime_error("DictKeyError: key not found");
    }
    return dict->l.at(h);
}

XObject* list_subscript(XObject* _l, XObject* i) {
    XObject* p = UNTAG(_l);
    XList* list = (XList*) UNTAG(_l);
    unsigned long index = PTR_TO_INT(i);
    if (index >= list->l.size()) {
        throw std::runtime_error("List index out of range");
    }
    return list->l[index];
}


XObject* f_String_add(XObject* _a, XObject* _b) {
    XString* a = (XString*) UNTAG(_a);
    XString* b = (XString*) UNTAG(_b);
    return GC::register_object(TAG(new XString(a->s + b->s)));
}

Function1 function_open_p = Function1(f_open);
Function1 function_print_p = Function1(f_print);
Function3 function_range_p = Function3(f_range);
Function2 function_map_p = Function2(f_map);
Function2 function_join_p = Function2(f_join);
Function1 function_File_read_line_p = Function1(f_File_read_line);
Function1 function_Integer_str_p = Function1(f_Integer_str);
Function1 function_Boolean_str_p = Function1(f_Boolean_str);
Function2 function_List_add_p = Function2(f_List_add);
Function1 function_List_len_p = Function1(f_List_len);
Function2 function_list_subscript_p = Function2(list_subscript);
Function2 function_String_add_p = Function2(f_String_add);

Function1* function_open = &function_open_p;
Function1* function_print = &function_print_p;
Function3* function_range = &function_range_p;
Function2* function_map = &function_map_p;
Function2* function_join = &function_join_p;
Function1* function_File_read_line = &function_File_read_line_p;
Function1* function_Integer_str = &function_Integer_str_p;
Function1* function_Boolean_str = &function_Boolean_str_p;
Function2* function_List_add = &function_List_add_p;
Function1* function_List_len = &function_List_len_p;
Function2* function_list_subscript = &function_list_subscript_p;
Function2* function_dict_subscript = nullptr;
Function2* function_String_add = &function_String_add_p;

Tuple::Tuple(const std::string& n, int num) : XObject(n), members(num, nullptr) {}

void Tuple::mark(std::vector<XObject*>& new_root) {
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


Tuple2::Tuple2(XObject* mem_1, XObject* mem_2) : Tuple("Tuple2", 2) {
    this->members[0] = mem_1;
    this->members[1] = mem_2;
}

XObject* Tuple2::get_member(int i) {
    assert(i >= 1 && i <= 2);
    return this->members[i - 1];
}

Tuple3::Tuple3(XObject* mem_1, XObject* mem_2, XObject* mem_3) : Tuple("Tuple3", 3) {
    this->members[0] = mem_1;
    this->members[1] = mem_2;
    this->members[2] = mem_3;
}

XObject* Tuple3::get_member(int i) {
    assert(i >= 1 && i <= 3);
    return this->members[i - 1];
}
