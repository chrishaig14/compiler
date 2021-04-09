//
// Created by chris on 5/12/20.
//

#include <ctime>
#include <bitset>
#include <cstring>
#include <sstream>
#include "core.h"
#include "xobjects/XString.h"
#include "xobjects/XFile.h"
#include "xobjects/XList.h"
#include "xobjects/XDict.h"
#include "xobjects/XTuple.h"

TaggedObject* core_D_open_f(TaggedObject* _s) {
    return NEW(XFile, (((XString*) UNTAG(_s))->s));
}

TaggedObject* core_D_print_f(TaggedObject* _s) {
    GC::declare(_s);
    XString* s = CAST(_s, XString);
    std::cout << s->s << std::endl;
    GC::out_of_scope(_s);
    return nullptr;
}

TaggedObject* core_D_range_f(TaggedObject* _start, TaggedObject* _step, TaggedObject* _end) {
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

TaggedObject* core_D_Integer_str_f(TaggedObject* _i) {
    TaggedObject* x = NEW(XString, std::to_string((GET_INT(_i))));
    return x;
}

TaggedObject* core_D_Double_str_f(TaggedObject* _i) {
    std::ostringstream out;
    out.precision(50);
    out << CAST(_i, XDouble)->d;
    std::string s;
    s = out.str();
    TaggedObject* x = NEW(XString, s);
    return x;
}

void foo(float f) {
    std::cout << f << std::endl;
}

TaggedObject* float_mk(float t) {
    TaggedObject* r = nullptr;
    memcpy((float*) &r + 1, &t, sizeof(float));
    r = (TaggedObject*) ((unsigned long) r | FLOAT_TAG);
    return r;
}

float float_rd(TaggedObject* r) {
    float e = 0;
    memcpy(&e, (float*) &r + 1, sizeof(float));
    return e;
}


TaggedObject* core_D_Float_str_f(TaggedObject* _i) {
    std::ostringstream out;
    out.precision(50);
    out << float_rd(_i);
    std::string s;
    s = out.str();
    TaggedObject* x = NEW(XString, s);
    return x;
}

TaggedObject* core_D_Boolean_str_f(TaggedObject* _i) {
    TaggedObject* x = NEW(XString, ((GET_BOOL(_i) ? "true" : "false")));
    return x;
}

TaggedObject* core_D_map_f(TaggedObject* _l, TaggedObject* _f) {
    GC::declare(_l);
    XList* l = CAST(_l, XList);
    Function1* f = CAST(_f, Function1);
    TaggedObject* _r = NEW(XList, l->l->size());
    XList* r = CAST(_r, XList);
    for (int i = 0; i < l->l->size(); i++) {
        TaggedObject* p = (*f)((*(l->l))[i]);
        r->l->at(i) = p;
    }
    GC::out_of_scope(_l);
    return _r;
    // return NEW(XList, r);
}

TaggedObject* core_D_join_f(TaggedObject* _l, TaggedObject* _s) {
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

Function1 core_D_open_o = Function1(core_D_open_f);
Function1 core_D_print_o = Function1(core_D_print_f);
Function3 core_D_range_o = Function3(core_D_range_f);
Function2 core_D_map_o = Function2(core_D_map_f);
Function2 core_D_join_o = Function2(core_D_join_f);
Function1 m_core_c_Integer_f_str_o = Function1(core_D_Integer_str_f);
Function1 m_core_c_Double_f_str_o = Function1(core_D_Double_str_f);
Function1 m_core_c_Float_f_str_o = Function1(core_D_Float_str_f);
Function1 m_core_c_Boolean_f_str_o = Function1(core_D_Boolean_str_f);
Function2 m_core_list_subscript_o = Function2(list_subscript);
Function2 m_core_string_subscript_o = Function2(string_subscript);
Function2 m_core_dict_subscript_o = Function2(dict_subscript);
Function2 core_D_Integer_D_add_o = Function2(core_D_Integer_D_add_f);
TaggedObject* core_D_open = FTAG(&core_D_open_o);
TaggedObject* core_D_print = FTAG(&core_D_print_o);
TaggedObject* core_D_range = FTAG(&core_D_range_o);
TaggedObject* core_D_map = FTAG(&core_D_map_o);
TaggedObject* core_D_join = FTAG(&core_D_join_o);
TaggedObject* core_Dile_read_line = FTAG(&m_core_c_File_f_read_line_o);
TaggedObject* m_core_c_Integer_f_str = FTAG(&m_core_c_Integer_f_str_o);
TaggedObject* m_core_c_Double_f_str = FTAG(&m_core_c_Double_f_str_o);
TaggedObject* m_core_c_Float_f_str = FTAG(&m_core_c_Float_f_str_o);
TaggedObject* m_core_c_Boolean_f_str = FTAG(&m_core_c_Boolean_f_str_o);
TaggedObject* m_core_list_subscript = FTAG(&m_core_list_subscript_o);
TaggedObject* m_core_string_subscript = FTAG(&m_core_string_subscript_o);
TaggedObject* m_core_dict_subscript = FTAG(&m_core_dict_subscript_o);


TaggedObject* core_D_Integer_D_add = FTAG(&core_D_Integer_D_add_o);

TaggedObject* core_D_Integer_D_add_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) + GET_INT(b));
}


