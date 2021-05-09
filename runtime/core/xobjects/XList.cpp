//
// Created by chris on 3/3/21.
//

#include "XList.h"
#include "../basics.h"

DEFINE_FUNCTION(1, core_D_List_D_len)
DEFINE_FUNCTION(2, core_D_List_D_push)
DEFINE_FUNCTION(2, core_D_List_D___item__)

TaggedObject* m_core_c_List_f_add_f(TaggedObject* _a, TaggedObject* _b) {
    GC::declare(_a);
    GC::declare(_b);
    XList* a = CAST(_a, XList);
    XList* b = CAST(_b, XList);
    XList* r = CAST(NEW(XList, a->l->size() + b->l->size()), XList);
    auto lv = r->lv;
    int sa = a->l->size();
    int sb = b->l->size();
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
    return MAKE_INT(l->l->size());
}

Function2 m_core_c_List_f_add_o = Function2(m_core_c_List_f_add_f);
Function1 m_core_c_List_f_len_o = Function1(m_core_c_List_f_len_f);
TaggedObject* m_core_c_List_f_add = FTAG(&m_core_c_List_f_add_o);
TaggedObject* m_core_c_List_f_len = FTAG(&m_core_c_List_f_len_o);

XList::XList(int n) : XObject("XList"), lv(n, nullptr) {
    this->l = &this->lv;
}


TaggedObject* XList::__eq__(TaggedObject* pObject) {
    XList* other = CAST(pObject, XList);
    for (size_t i = 0; i < this->l->size(); i++) {
        if (EQ((*this->l)[i], (*other->l)[i]) == MAKE_BOOL(false)) {
            return MAKE_BOOL(false);
        }
    }
    return MAKE_BOOL(true);
}

XList::~XList() {
    if (this->l->size() != 0) {
        if (has_tag(this->l->at(0), OBJECT_TAG)) {
            for (size_t i = 0; i < this->l->size(); i++) {
                if (has_tag(this->l->at(i), FUNCTION_TAG)) {
                    continue;
                }
                XObject* el = UNTAG(this->l->at(i));
                el->dec_count();
                if (el->count == 0) {
                    delete el;
                }
            }
        }
    }
    std::cout << "delete list of length " << this->l->size() << std::endl;
}

TaggedObject* core_D_List_D_len_f(TaggedObject* a) {
    return MAKE_INT(CAST(a, XList)->lv.size());
}

TaggedObject* core_D_List_D___item___f(TaggedObject* a, TaggedObject* b) {
    long idx = GET_INT(b);
    unsigned long list_len = CAST(a, XList)->lv.size();
    if (idx >= list_len) {
        throw std::runtime_error("List index " + std::to_string(idx) +
                                 (" out of range of list with length " + std::to_string(list_len)));
    }
    return CAST(a, XList)->lv[idx];
}

TaggedObject* core_D_List_D_push_f(TaggedObject* a, TaggedObject* b) {
    CAST(a, XList)->lv.push_back(b);
    GC::declare(b);
    return nullptr;
}
