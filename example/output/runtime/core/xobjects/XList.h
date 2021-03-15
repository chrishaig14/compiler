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
    void inc_count() override;
    void dec_count() override;
};

TaggedObject* f_List_add(TaggedObject* _a, TaggedObject* _b);
TaggedObject* f_List_len(TaggedObject* _l);
extern TaggedObject* function_List_add;
extern TaggedObject* function_List_len;
extern Function2 function_List_add_p;
extern Function1 function_List_len_p;


#endif //XLANG_XLIST_H
