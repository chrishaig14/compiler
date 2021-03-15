//
// Created by chris on 3/3/21.
//

#include "XList.h"
#include "../basics.h"

TaggedObject* f_List_add(TaggedObject* _a, TaggedObject* _b) {
    GC::declare(_a);
    GC::declare(_b);
    XList* a = CAST(_a, XList);
    XList* b = CAST(_b, XList);
    XList* r = CAST(NEW(XList, a->l->size() + b->l->size()), XList);
    auto lv = r->lv;
    int sa = a->l->size();
    int sb = b->l->size();
    for (int i = 0; i < sa; i++) {
        lv[i] = a->lv[i];
    }
    for (int i = sa; i < sb; i++) {
        lv[i] = b->lv[i - sa];
    }
    GC::assign(_a, nullptr);
    GC::assign(_b, nullptr);
    return TAG(r);
}

TaggedObject* f_List_len(TaggedObject* _l) {
    XList* l = (XList*) (UNTAG(_l));
    return MAKE_INT(l->l->size());
}

Function2 function_List_add_p = Function2(f_List_add);
Function1 function_List_len_p = Function1(f_List_len);
TaggedObject* function_List_add = TAG(&function_List_add_p);
TaggedObject* function_List_len = TAG(&function_List_len_p);

XList::XList(int n) : XObject("XList"), lv(n, nullptr) {
    this->l = &this->lv;
}


TaggedObject* XList::__eq__(TaggedObject* pObject) {
    XList* other = CAST(pObject, XList);
    for (int i = 0; i < this->l->size(); i++) {
        if (EQ((*this->l)[i], (*other->l)[i]) == MAKE_BOOL(false)) {
            return MAKE_BOOL(false);
        }
    }
    return MAKE_BOOL(true);
}

XList::~XList() {
    if (this->l->size() != 0) {
        if (has_tag(this->l->at(0), OBJECT_TAG)) {
            for (int i = 0; i < this->l->size(); i++) {
                XObject* el = UNTAG(this->l->at(i));
                std::cout << "el: " << el << std::endl;
                std::cout << "NOW COUNT IS: " << el->count << std::endl;
                el->dec_count();
                if (el->count == 0) {
                    delete el;
                }
            }
        }
    }
    std::cout << "delete list of length " << this->l->size() << std::endl;
}

void XList::inc_count() {
    this->count++;
}

void XList::dec_count() {
    this->count--;
}

