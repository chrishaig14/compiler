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
    auto ab = a->l->begin();
    auto ae = a->l->end();
    auto* l = new std::vector<TaggedObject*>();
    l->insert(l->end(), ab, ae);
    auto be = b->l->end();
    auto bb = b->l->begin();
    l->insert(l->end(), bb, be);
    XList* r = CAST(NEW(XList, l), XList);
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

XList::XList(std::vector<TaggedObject*>* v) : XObject("List"), l(v) {
    for (auto e: *l) {
        if (!has_tag(e, OBJECT_TAG)) {
            break;
        }
        UNTAG(e)->inc_count();
    }
    this->is_list = true;
}

void XList::mark(std::vector<XObject*>& new_root) {
    int list_len = this->l->size();
    if (list_len != 0) {
        if (has_tag((*this->l)[0], INT_TAG)) {
            // don't gc ints as they are not heap-allocated
        } else {
            for (int j = 0; j < list_len; j++) {
                XObject* element = PTR_TO_OBJ((*this->l)[j]);
                if (!element->is_reachable() && !element->inserted) {
                    element->set_reachable();
                    new_root.push_back(element);
                    element->inserted = true;
                }
            }
        }
    }
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
    delete this->l;
}

void XList::inc_count() {
    this->count++;
    // if (this->l->size() != 0) {
    //     if (has_tag(this->l->at(0), OBJECT_TAG)) {
    //         for (int i = 0; i < this->l->size(); i++) {
    //             XObject* el = UNTAG(this->l->at(i));
    //             el->inc_count();
    //         }
    //     }
    // }
}

void XList::dec_count() {
    this->count--;
    // if (this->l->size() != 0) {
    //     if (has_tag(this->l->at(0), OBJECT_TAG)) {
    //         for (int i = 0; i < this->l->size(); i++) {
    //             XObject* el = UNTAG(this->l->at(i));
    //             el->dec_count();
    //         }
    //     }
    // }
}
