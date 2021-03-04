//
// Created by chris on 3/3/21.
//

#include "XList.h"

XObject* f_List_add(XObject* _a, XObject* _b) {
    XList* a = CAST(_a, XList);
    XList* b = CAST(_b, XList);
    XList* r = CAST(NEW(XList, std::vector<XObject*>()), XList);
    auto re = r->l.end();
    auto ab = a->l.begin();
    auto ae = a->l.end();
    r->l.insert(re, ab, ae);
    const std::vector<XObject*>::iterator& be = b->l.end();
    const std::vector<XObject*>::iterator& bb = b->l.begin();
    const std::vector<XObject*>::iterator& re2 = r->l.end();
    r->l.insert(re2, bb, be);
    return TAG(r);
}

XObject* f_List_len(XObject* _l) {
    XList* l = (XList*) (UNTAG(_l));
    return INT_TO_PTR(l->l.size());
}

Function2 function_List_add_p = Function2(f_List_add);
Function1 function_List_len_p = Function1(f_List_len);
Function2* function_List_add = &function_List_add_p;
Function1* function_List_len = &function_List_len_p;

XList::XList(const std::vector<XObject*> v) : XObject("List") {
    this->l = v;
    this->is_list = true;
}

void XList::mark(std::vector<XObject*>& new_root) {
    int list_len = this->l.size();
    if (list_len != 0) {
        if (has_tag(this->l[0], INT_TAG)) {
            // don't gc ints as they are not heap-allocated
        } else {
            for (int j = 0; j < list_len; j++) {
                XObject* element = PTR_TO_OBJ(this->l[j]);
                if (!element->is_reachable() && !element->inserted) {
                    element->set_reachable();
                    new_root.push_back(element);
                    element->inserted = true;
                }
            }
        }
    }
}
