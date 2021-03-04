//
// Created by chris on 5/12/20.
//

#include "core.h"
#include "xobjects/XString.h"
#include "xobjects/XFile.h"
#include "xobjects/XList.h"
#include "xobjects/XDict.h"
#include "xobjects/XTuple.h"

XObject* f_open(XObject* _s) {
    return new XFile(((XString*) UNTAG(_s))->s);
}

XObject* f_print(XObject* _s) {
    XString* s = CAST(_s, XString);
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
        XTuple* ta = (XTuple*) oa;
        XTuple* tb = (XTuple*) ob;
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

XObject* string_subscript(XObject* _l, XObject* i) {
    XObject* p = UNTAG(_l);
    XString* str = (XString*) UNTAG(_l);
    unsigned long index = PTR_TO_INT(i);
    std::cout << "String is: " << str->s << std::endl;
    if (index >= str->s.size()) {
        throw std::runtime_error("String index out of range: " + std::to_string(index) + " but length is " +
                                 std::to_string(str->s.size()));
    }
    return NEW(XString, std::string(1, str->s[index]));
}

Function1 function_open_p = Function1(f_open);
Function1 function_print_p = Function1(f_print);
Function3 function_range_p = Function3(f_range);
Function2 function_map_p = Function2(f_map);
Function2 function_join_p = Function2(f_join);
Function1 function_Integer_str_p = Function1(f_Integer_str);
Function1 function_Boolean_str_p = Function1(f_Boolean_str);
Function2 function_list_subscript_p = Function2(list_subscript);
Function2 function_string_subscript_p = Function2(string_subscript);
Function1* function_open = &function_open_p;
Function1* function_print = &function_print_p;
Function3* function_range = &function_range_p;
Function2* function_map = &function_map_p;
Function2* function_join = &function_join_p;
Function1* function_File_read_line = &function_File_read_line_p;
Function1* function_Integer_str = &function_Integer_str_p;
Function1* function_Boolean_str = &function_Boolean_str_p;
Function2* function_list_subscript = &function_list_subscript_p;
Function2* function_string_subscript = &function_string_subscript_p;
Function2* function_dict_subscript = nullptr;


