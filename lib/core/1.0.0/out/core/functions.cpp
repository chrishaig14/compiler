//
// Created by chris on 25/4/21.
//

#include "functions.h"

DEFINE_FUNCTION(1, core_D_core_D_print)
DEFINE_FUNCTION(1, core_D_exit)
DEFINE_FUNCTION(3, core_D_core_D_range)
DEFINE_FUNCTION(2, core_D_map)
DEFINE_FUNCTION(2, core_D_join)

TaggedObject* core_D_exit_f(TaggedObject* c) {
    exit(GET_INT(c));
    return nullptr;
}

TaggedObject* core_D_core_D_print_f(TaggedObject* _s) {
    GC::declare(_s);
    XString* s = CAST(_s, XString);
    std::cout << s->s << std::endl;
    GC::out_of_scope(_s);
    return nullptr;
}

TaggedObject* core_D_core_D_range_f(TaggedObject* _start, TaggedObject* _step, TaggedObject* _end) {
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


TaggedObject* core_D_map_f(TaggedObject* _l, TaggedObject* _f) {
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

TaggedObject* core_D_join_f(TaggedObject* _l, TaggedObject* _s) {
    std::vector<TaggedObject*>* l = (std::vector<TaggedObject*>*) _l;
    std::string s;
    std::string js = *(std::string*) (_s);
    for (size_t i = 0; i < l->size(); i++) {
        s += *(std::string*) ((*l)[i]) + js;
    }
    s = s.substr(0, s.size() - js.size());
    return NEW(XString, s);
}