//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XLIST_H
#define XLANG_XLIST_H

#include "XObject.h"
#include "../xobject.h"

class XList : public XObject {
public:
    std::vector<XObject*> l;

    XList(const std::vector<XObject*> v) : XObject("List") {
        this->l = v;
        this->is_list = true;
    }

    void mark(std::vector<XObject*>& new_root) override {
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
};

#endif //XLANG_XLIST_H
