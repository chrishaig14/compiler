//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XLIST_H
#define XLANG_XLIST_H

#include "XObject.h"
#include "../Function.h"
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


extern TaggedObject* core_D_List_D_len;
extern TaggedObject* core_D_List_D___sub__;
TaggedObject* core_D_List_D_len_f(TaggedObject* a);

TaggedObject* core_D_List_D___sub___f(TaggedObject* a, TaggedObject* b);
extern Function2 core_D_List_D___sub__o;

extern Function1 core_D_List_D_len_o;


#endif //XLANG_XLIST_H
