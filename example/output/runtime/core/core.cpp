//
// Created by chris on 5/12/20.
//

#include "core.h"
#include "xobjects/XString.h"
#include "xobjects/XFile.h"
#include "xobjects/XList.h"
#include "xobjects/XDict.h"
#include "xobjects/XTuple.h"

TaggedObject* f_open(TaggedObject* _s) {
    return NEW(XFile, (((XString*) UNTAG(_s))->s));
}

TaggedObject* f_print(TaggedObject* _s) {
    XString* s = CAST(_s, XString);
    std::cout << s->s << std::endl;
    return nullptr;
}

TaggedObject* f_range(TaggedObject* _start, TaggedObject* _step, TaggedObject* _end) {
    long start = GET_INT(_start);
    long step = GET_INT(_step);
    long end = GET_INT(_end);
    std::vector<TaggedObject*> v((end - start) / step, nullptr);
    int k = 0;
    for (int i = start; i < end; i += step) {
        v[k] = MAKE_INT(i);
        k++;
    }
    return NEW(XList, v);
}

TaggedObject* f_Integer_str(TaggedObject* _i) {
    TaggedObject* x = NEW(XString, std::to_string((GET_INT(_i))));
    return x;
}

TaggedObject* f_Boolean_str(TaggedObject* _i) {
    TaggedObject* x = NEW(XString, ((GET_BOOL(_i) ? "true" : "false")));
    return x;
}

TaggedObject* f_map(TaggedObject* _l, TaggedObject* _f) {
    std::vector<TaggedObject*>* l = (std::vector<TaggedObject*>*) _l;
    Function1* f = (Function1*) (_f);
    std::vector<TaggedObject*> r;
    for (int i = 0; i < l->size(); i++) {
        r.push_back((*f)((*l)[i]));
    }
    return NEW(XList, r);
}

TaggedObject* f_join(TaggedObject* _l, TaggedObject* _s) {
    std::vector<TaggedObject*>* l = (std::vector<TaggedObject*>*) _l;
    std::string s;
    std::string js = *(std::string*) (_s);
    for (int i = 0; i < l->size(); i++) {
        s += *(std::string*) ((*l)[i]) + js;
    }
    s = s.substr(0, s.size() - js.size());
    return NEW(XString, s);
}

TaggedObject* op_lt(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) < GET_INT(b));
}

TaggedObject* op_gt(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) > GET_INT(b));
}

TaggedObject* op_eq(TaggedObject* a, TaggedObject* b) {
    if (has_tag(a, INT_TAG)) {
        return MAKE_BOOL(GET_INT(a) == GET_INT(b));
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
                if (!GET_BOOL(op_eq(la->l[i], lb->l[i]))) {
                    r = false;
                    break;
                }
            }
        }
    } else {
        XTuple* ta = (XTuple*) oa;
        XTuple* tb = (XTuple*) ob;
        for (int i = 0; i < ta->members.size(); i++) {
            if (!GET_BOOL(op_eq(ta->members[i], tb->members[i]))) {
                r = false;
                break;
            }
        }
    }
    return MAKE_BOOL(r);
}

TaggedObject* op_neq(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) != GET_INT(b));
}

TaggedObject* op_geq(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) >= GET_INT(b));
}


TaggedObject* op_leq(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) <= GET_INT(b));
}

TaggedObject* dict_subscript(TaggedObject* _l, TaggedObject* i) {
    XDict* dict = (XDict*) UNTAG(_l);
    int h = hash(i);
    if (dict->l.find(h) == dict->l.end()) {
        throw std::runtime_error("DictKeyError: key not found");
    }
    return dict->l.at(h);
}

TaggedObject* list_subscript(TaggedObject* _l, TaggedObject* i) {
    XList* list = (XList*) UNTAG(_l);
    unsigned long index = GET_INT(i);
    if (index >= list->l.size()) {
        throw std::runtime_error("List index out of range");
    }
    return list->l[index];
}

TaggedObject* string_subscript(TaggedObject* _l, TaggedObject* i) {
    XString* str = (XString*) UNTAG(_l);
    unsigned long index = GET_INT(i);
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
TaggedObject* function_open = TAG(&function_open_p);
TaggedObject* function_print = TAG(&function_print_p);
TaggedObject* function_range = TAG(&function_range_p);
TaggedObject* function_map = TAG(&function_map_p);
TaggedObject* function_join = TAG(&function_join_p);
TaggedObject* function_File_read_line = TAG(&function_File_read_line_p);
TaggedObject* function_Integer_str = TAG(&function_Integer_str_p);
TaggedObject* function_Boolean_str = TAG(&function_Boolean_str_p);
TaggedObject* function_list_subscript = TAG(&function_list_subscript_p);
TaggedObject* function_string_subscript = TAG(&function_string_subscript_p);
TaggedObject* function_dict_subscript = TAG(nullptr);


