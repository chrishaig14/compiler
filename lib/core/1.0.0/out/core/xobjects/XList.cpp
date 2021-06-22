//
// Created by chris on 3/3/21.
//

#include "XList.h"
#include "../basics.h"

DEFINE_FUNCTION(1, core_D_core_D_List_D_len)
DEFINE_FUNCTION(2, core_D_core_D_List_D_has)
DEFINE_FUNCTION(2, core_D_core_D_List_D_push)
DEFINE_FUNCTION(2, core_D_core_D_List_D___get_item__)
DEFINE_FUNCTION(3, core_D_core_D_List_D___set_item__)

DEFINE_FUNCTION(2, core_D_core_D_List_D___add__)

TaggedObject* m_core_c_List_f_add_f(TaggedObject* _a, TaggedObject* _b) {
    GC::declare(_a);
    GC::declare(_b);
    XList* a = CAST(_a, XList);
    XList* b = CAST(_b, XList);
    XList* r = CAST(XList_init_with_length(a->lv.size() + b->lv.size()), XList);
    auto lv = r->lv;
    int sa = a->lv.size();
    int sb = b->lv.size();
    for (size_t i = 0; i < sa; i++) {
        lv[i] = a->lv[i];
    }
    for (size_t i = sa; i < sb; i++) {
        lv[i] = b->lv[i - sa];
    }
    GC::assign(_a, nullptr);
    GC::assign(_b, nullptr);
    return TAG(r);
}

TaggedObject* m_core_c_List_f_len_f(TaggedObject* _l) {
    XList* l = (XList*) (UNTAG(_l));
    return MAKE_INT(l->lv.size());
}

Function2 m_core_c_List_f_add_o = Function2(m_core_c_List_f_add_f);
Function1 m_core_c_List_f_len_o = Function1(m_core_c_List_f_len_f);
TaggedObject* m_core_c_List_f_add = FTAG(&m_core_c_List_f_add_o);
TaggedObject* m_core_c_List_f_len = FTAG(&m_core_c_List_f_len_o);


TaggedObject* XList__eq__(TaggedObject* _this_obj, TaggedObject* pObject) {
    XList* this_obj = CAST(_this_obj, XList);
    XList* other = CAST(pObject, XList);
    for (size_t i = 0; i < this_obj->lv.size(); i++) {
        if (EQ(this_obj->lv[i], other->lv[i]) == MAKE_BOOL(false)) {
            return MAKE_BOOL(false);
        }
    }
    return MAKE_BOOL(true);
}

void XList_clean(TaggedObject* _this_obj) {
    XList* this_obj = CAST(_this_obj, XList);
    if (GC::collecting) {
        return;
    }
    if (this_obj->lv.size() != 0) {
        if (has_tag(this_obj->lv.at(0), OBJECT_TAG)) {
            for (size_t i = 0; i < this_obj->lv.size(); i++) {
                if (has_tag(this_obj->lv.at(i), FUNCTION_TAG)) {
                    continue;
                }
                GC::out_of_scope(this_obj->lv.at(i));
            }
        }
    }
    std::cout << "delete list of length " << this_obj->lv.size() << std::endl;
}


std::vector<XObject*> core_D_core_D_List_D_get_all_members(TaggedObject* _this_obj) {
    XList* this_obj = CAST(_this_obj, XList);
    if (this_obj->lv.empty()) {
        return {};
    }
    if (!has_tag(this_obj->lv[0], OBJECT_TAG)) {
        return {};
    }
    std::vector<XObject*> ret;
    for (auto* to: this_obj->lv) {
        ret.push_back(UNTAG(to));
    }
    return ret;
}

TaggedObject* core_D_core_D_List_D_len_f(TaggedObject* a) {
    return MAKE_INT(CAST(a, XList)->lv.size());
}

TaggedObject* core_D_core_D_List_D___get_item___f(TaggedObject* a, TaggedObject* b) {
    long idx = GET_INT(b);
    unsigned long list_len = CAST(a, XList)->lv.size();
    if (idx >= list_len) {
        throw std::runtime_error("List index " + std::to_string(idx) +
                                 (" out of range of list with length " + std::to_string(list_len)));
    }
    return CAST(a, XList)->lv[idx];
}

TaggedObject* core_D_core_D_List_D___set_item___f(TaggedObject* a, TaggedObject* b, TaggedObject* c) {
    long idx = GET_INT(b);
    unsigned long list_len = CAST(a, XList)->lv.size();
    if (idx >= list_len) {
        throw std::runtime_error("List index " + std::to_string(idx) +
                                 (" out of range of list with length " + std::to_string(list_len)));
    }
    CAST(a, XList)->lv[idx] = c;
    return nullptr;
}

TaggedObject* core_D_core_D_List_D___add___f(TaggedObject* a, TaggedObject* b) {
    XList* _a = CAST(a, XList);
    XList* _b = CAST(b, XList);
    TaggedObject* res = XList_init_with_length(_a->lv.size() + _b->lv.size());
    int i = 0;
    std::vector<TaggedObject*>& res_vec = CAST(res, XList)->lv;
    for (auto xa: _a->lv) {
        res_vec[i] = xa;
        i++;
    }
    for (auto xb: _b->lv) {
        res_vec[i] = xb;
        i++;
    }
    return res;
}

TaggedObject* core_D_core_D_List_D_has_f(TaggedObject* a, TaggedObject* b) {
    GC::declare(b);
    for (auto e: CAST(a, XList)->lv) {
        if (has_tag(e, OBJECT_TAG)) {
            if (GET_BOOL(UNTAG(e)->__eq__(b))) {
                GC::out_of_scope(b);
                return TRUE;
            }
        } else if (e == b) {
            GC::out_of_scope(b);
            return TRUE;
        }
    }
    GC::out_of_scope(b);
    return FALSE;
}

TaggedObject* core_D_core_D_List_D_push_f(TaggedObject* a, TaggedObject* b) {
    CAST(a, XList)->lv.push_back(b);
    GC::declare(b);
    return nullptr;
}

DEFINE_FUNCTION(2, core_D_core_D_List_D_map)

TaggedObject* core_D_core_D_List_D_map_f(TaggedObject* a, TaggedObject* f) {
    GC::declare(a);
    GC::declare(f);
    XList* la = CAST(a, XList);
    TaggedObject* r = XList_init_with_length(la->lv.size());
    XList* rl = CAST(r, XList);
    for (int i = 0; i < la->lv.size(); i++) {
        TaggedObject* re = CALL1(f, la->lv[i]);
        rl->lv[i] = GC::declare(re);
    }
    GC::out_of_scope(a);
    GC::out_of_scope(f);
    return r;
}

DEFINE_FUNCTION(2, core_D_core_D_List_D_where)

TaggedObject* core_D_core_D_List_D_where_f(TaggedObject* a, TaggedObject* f) {
    GC::declare(a);
    GC::declare(f);
    XList* la = CAST(a, XList);
    TaggedObject* r = NEW(XList, {});
    XList* rl = CAST(r, XList);
    for (int i = 0; i < la->lv.size(); i++) {
        TaggedObject* re = CALL1(f, la->lv[i]);
        if (GET_BOOL(re)) {
            rl->lv.push_back(GC::declare(la->lv[i]));
        }
    }
    GC::out_of_scope(a);
    GC::out_of_scope(f);
    return r;
}

Vtable XList_vtable = {XList_clean, core_D_core_D_List_D_get_all_members, nullptr};

TaggedObject* XList_init_with_length(size_t n) {
    XList* l = new XList;
    l->lv.reserve(n);
    l->vtable = &XList_vtable;
    return TAG(l);
}


TaggedObject* core_D_core_D_List_D__init__f(const std::initializer_list<TaggedObject*>& c) {
    XList* this_obj = new XList{c, &XList_vtable};
    if (this_obj->lv.size() != 0) {
        if (has_tag(this_obj->lv[0], OBJECT_TAG)) {
            for (auto& e: this_obj->lv) {
                UNTAG(e)->inc_count();
            }
        }
    }
    return TAG(this_obj);
}
