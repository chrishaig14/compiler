//
// Created by chris on 3/3/21.
//

#include "XList.h"
#include "../basics.h"

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
