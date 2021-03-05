//
// Created by chris on 3/3/21.
//

#include "XList.h"

TaggedObject* f_List_add(TaggedObject* _a, TaggedObject* _b) {
    XList* a = CAST(_a, XList);
    XList* b = CAST(_b, XList);
    XList* r = CAST(NEW(XList, std::vector<TaggedObject*>()), XList);
    auto re = r->l.end();
    auto ab = a->l.begin();
    auto ae = a->l.end();
    r->l.insert(re, ab, ae);
    const std::vector<TaggedObject*>::iterator& be = b->l.end();
    const std::vector<TaggedObject*>::iterator& bb = b->l.begin();
    const std::vector<TaggedObject*>::iterator& re2 = r->l.end();
    r->l.insert(re2, bb, be);
    return TAG(r);
}

TaggedObject* f_List_len(TaggedObject* _l) {
    XList* l = (XList*) (UNTAG(_l));
    return INT_TO_PTR(l->l.size());
}

Function2 function_List_add_p = Function2(f_List_add);
Function1 function_List_len_p = Function1(f_List_len);
TaggedObject* function_List_add = TAG(&function_List_add_p);
TaggedObject* function_List_len = TAG(&function_List_len_p);

XList::XList(const std::vector<TaggedObject*> v) : XObject("List") {
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
