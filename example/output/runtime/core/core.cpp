//
// Created by chris on 5/12/20.
//

#include <ctime>
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
    GC::declare(_s);
    XString* s = CAST(_s, XString);
    std::cout << s->s << std::endl;
    GC::out_of_scope(_s);
    return nullptr;
}

TaggedObject* f_range(TaggedObject* _start, TaggedObject* _step, TaggedObject* _end) {
    long start = GET_INT(_start);
    long step = GET_INT(_step);
    long end = GET_INT(_end);

    XList* r = CAST(NEW(XList, (end - start) / step), XList);
    std::vector<TaggedObject*>* v = &r->lv;
    int k = 0;
    for (int i = start; i < end; i += step) {
        (*v)[k] = MAKE_INT(i);
        k++;
    }
    r->l = &r->lv;
    return TAG(r);
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
    GC::declare(_l);
    XList* l = CAST(_l, XList);
    Function1* f = CAST(_f, Function1);
    TaggedObject* _r = NEW(XList, l->l->size());
    XList* r = CAST(_r, XList);
    for (int i = 0; i < l->l->size(); i++) {
        TaggedObject* p = (*f)((*(l->l))[i]);
        r->l->at(i)=p;
    }
    GC::out_of_scope(_l);
    return _r;
    // return NEW(XList, r);
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
    if (index >= list->l->size()) {
        throw std::runtime_error("List index out of range");
    }
    return (*list->l)[index];
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
Function2 function_dict_subscript_p = Function2(dict_subscript);
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
TaggedObject* function_dict_subscript = TAG(&function_dict_subscript_p);


