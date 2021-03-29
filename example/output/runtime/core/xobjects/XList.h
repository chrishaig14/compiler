//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XLIST_H
#define XLANG_XLIST_H

#include "XObject.h"
#include "../functions.h"
#include "../../macros.h"
#include "../GC.h"

class XList : public XObject {
public:
    std::vector<TaggedObject*> lv;
    std::vector<TaggedObject*>* l;

    XList(int n);

    XList(const std::initializer_list<TaggedObject*>& c) : XObject("List"), lv(c) {
        this->l = &this->lv;
        if (has_tag(this->lv[0], OBJECT_TAG)) {
            for (auto& e: this->lv) {
                UNTAG(e)->inc_count();
            }
        }

    }

    TaggedObject* __eq__(TaggedObject* pObject) override;
    ~XList() override;
};

TaggedObject* m_core_c_List_f_add_f(TaggedObject* _a, TaggedObject* _b);
TaggedObject* m_core_c_List_f_len_f(TaggedObject* _l);
extern TaggedObject* m_core_c_List_f_add;
extern TaggedObject* m_core_c_List_f_len;


#endif //XLANG_XLIST_H
