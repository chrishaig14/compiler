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

TaggedObject*  core_D_open_f(TaggedObject* _s) {
    return NEW(XFile, (((XString*) UNTAG(_s))->s));
}

TaggedObject*  core_D_print_f(TaggedObject* _s) {
    GC::declare(_s);
    XString* s = CAST(_s, XString);
    std::cout << s->s << std::endl;
    GC::out_of_scope(_s);
    return nullptr;
}

TaggedObject*  core_D_range_f(TaggedObject* _start, TaggedObject* _step, TaggedObject* _end) {
    long start = GET_INT(_start);
    long step = GET_INT(_step);
    long end = GET_INT(_end);

    XList* r = CAST(NEW(XList, (end - start) / step), XList);
    std::vector<TaggedObject*>* v = &r->lv;
    int k = 0;
    for (size_t i = start; i < end; i += step) {
        (*v)[k] = MAKE_INT(i);
        k++;
    }
    r->l = &r->lv;
    return TAG(r);
}

TaggedObject*  core_D_Integer_D_str_f(TaggedObject* _i) {
    TaggedObject* x = NEW(XString, std::to_string((GET_INT(_i))));
    return x;
}

TaggedObject*  core_D_Double_D_str_f(TaggedObject* _i) {
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


TaggedObject*  core_D_Float_D_str_f(TaggedObject* _i) {
    std::ostringstream out;
    out.precision(50);
    out << float_rd(_i);
    std::string s;
    s = out.str();
    TaggedObject* x = NEW(XString, s);
    return x;
}

TaggedObject*  core_D_Boolean_D_str_f(TaggedObject* _i) {
    TaggedObject* x = NEW(XString, ((GET_BOOL(_i) ? "true" : "false")));
    return x;
}

TaggedObject*  core_D_map_f(TaggedObject* _l, TaggedObject* _f) {
    GC::declare(_l);
    XList* l = CAST(_l, XList);
    Function1* f = CAST(_f, Function1);
    TaggedObject* _r = NEW(XList, l->l->size());
    XList* r = CAST(_r, XList);
    for (size_t i = 0; i < l->l->size(); i++) {
        TaggedObject* p = (*f)((*(l->l))[i]);
        r->l->at(i) = p;
    }
    GC::out_of_scope(_l);
    return _r;
    // return NEW(XList, r);
}

TaggedObject*  core_D_join_f(TaggedObject* _l, TaggedObject* _s) {
    std::vector<TaggedObject*>* l = (std::vector<TaggedObject*>*) _l;
    std::string s;
    std::string js = *(std::string*) (_s);
    for (size_t i = 0; i < l->size(); i++) {
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

TaggedObject*  core_D_dict_subscript_f(TaggedObject* _l, TaggedObject* i) {
    XDict* dict = (XDict*) UNTAG(_l);
    int h = hash(i);
    if (dict->l.find(h) == dict->l.end()) {
        throw std::runtime_error("DictKeyError: key not found");
    }
    return dict->l.at(h);
}

TaggedObject*  core_D_list_subscript_f(TaggedObject* _l, TaggedObject* i) {
    XList* list = (XList*) UNTAG(_l);
    unsigned long index = GET_INT(i);
    if (index >= list->l->size()) {
        throw std::runtime_error("List index out of range");
    }
    return (*list->l)[index];
}

TaggedObject*  core_D_string_subscript_f(TaggedObject* _l, TaggedObject* i) {
    XString* str = (XString*) UNTAG(_l);
    unsigned long index = GET_INT(i);
    std::cout << "String is: " << str->s << std::endl;
    if (index >= str->s.size()) {
        throw std::runtime_error("String index out of range: " + std::to_string(index) + " but length is " +
                                 std::to_string(str->s.size()));
    }
    return NEW(XString, std::string(1, str->s[index]));
}

TaggedObject*  core_D_Integer_D_add_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) + GET_INT(b));
}

TaggedObject*  core_D_Integer_D_sub_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) - GET_INT(b));
}

TaggedObject*  core_D_Integer_D_div_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) / GET_INT(b));
}

TaggedObject*  core_D_Integer_D_mul_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_INT(GET_INT(a) * GET_INT(b));
}

TaggedObject*  core_D_String_D_add_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_STRING(CAST(a, XString)->s + CAST(b, XString)->s);
}

TaggedObject*  core_D_String_D___sub___f(TaggedObject* a, TaggedObject* b) {
    if (GET_INT(b) >= CAST(a, XString)->s.size()) {
        throw std::runtime_error("Error subscript of String greater than length!");
    }
    return MAKE_STRING(std::string(1, CAST(a, XString)->s[GET_INT(b)]));
}


#define FUNCTION_RAW_NAME(x) x##_f
#define FUNCTION_OBJECT_NAME(x) x##_o
#define FUNCTION_OBJECT(n, x) Function##n FUNCTION_OBJECT_NAME(x) = Function##n(FUNCTION_RAW_NAME(x));
#define DEFINE_FUNCTION(n, x) FUNCTION_OBJECT(n, x) TaggedObject* x = FTAG(&FUNCTION_OBJECT_NAME(x));

DEFINE_FUNCTION(1,  core_D_open)
DEFINE_FUNCTION(1,  core_D_print)
DEFINE_FUNCTION(3,  core_D_range)
DEFINE_FUNCTION(2,  core_D_map)
DEFINE_FUNCTION(2,  core_D_join)
DEFINE_FUNCTION(2,  core_D_String_D_add)
DEFINE_FUNCTION(2,  core_D_Integer_D_add)
DEFINE_FUNCTION(2,  core_D_Integer_D_sub)
DEFINE_FUNCTION(2,  core_D_Integer_D_div)
DEFINE_FUNCTION(2,  core_D_Integer_D_mul)
DEFINE_FUNCTION(1,  core_D_Integer_D_str)

DEFINE_FUNCTION(2,  core_D_Integer_D_lt)
DEFINE_FUNCTION(2,  core_D_Integer_D_gt)
DEFINE_FUNCTION(2,  core_D_Integer_D_le)
DEFINE_FUNCTION(2,  core_D_Integer_D_ge)
DEFINE_FUNCTION(2,  core_D_Integer_D_eq)
DEFINE_FUNCTION(2,  core_D_Integer_D_ne)



DEFINE_FUNCTION(1,  core_D_Double_D_str)
DEFINE_FUNCTION(1,  core_D_Float_D_str)
DEFINE_FUNCTION(1,  core_D_Boolean_D_str)
DEFINE_FUNCTION(2,  core_D_List_D___sub__)
DEFINE_FUNCTION(1,  core_D_List_D_len)
DEFINE_FUNCTION(1,  core_D_String_D_len)


DEFINE_FUNCTION(2,  core_D_String_D___sub__)

TaggedObject*  core_D_Integer_D_lt_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) < GET_INT(b));
}

TaggedObject*  core_D_Integer_D_gt_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) > GET_INT(b));
}

TaggedObject*  core_D_Integer_D_le_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) <= GET_INT(b));
}

TaggedObject*  core_D_Integer_D_ge_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) >= GET_INT(b));
}

TaggedObject*  core_D_Integer_D_ne_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) != GET_INT(b));
}

TaggedObject*  core_D_Integer_D_eq_f(TaggedObject* a, TaggedObject* b) {
    return MAKE_BOOL(GET_INT(a) == GET_INT(b));
}

TaggedObject*  core_D_List_D_len_f(TaggedObject* a) {
    return MAKE_INT(CAST(a, XList)->lv.size());
}

TaggedObject*  core_D_List_D___sub___f(TaggedObject* a, TaggedObject* b) {
    return CAST(a, XList)->lv[GET_INT(b)];
}

TaggedObject* core_D_String_D_len_f(TaggedObject* a) {
    return MAKE_INT(CAST(a, XString)->s.size());
}

DEFINE_FUNCTION(2,  core_D_dict_subscript)

