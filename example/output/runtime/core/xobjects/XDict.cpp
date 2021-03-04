//
// Created by chris on 3/3/21.
//

#include "XDict.h"


XDict::XDict(std::unordered_map<XObject*, XObject*> v) : XObject("Dict") {
    for (auto& it: v) {
        this->l[hash(it.first)] = it.second;
    }
    this->is_list = false;
}

void XDict::mark(std::vector<XObject*>& new_root) {
    // int list_len = this->l.size();
    // if (list_len != 0) {
    //     if (has_tag(this->l[0], INT_TAG)) {
    //         // don't gc ints as they are not heap-allocated
    //     } else {
    //         for (int j = 0; j < list_len; j++) {
    //             XObject* element = PTR_TO_OBJ(this->l[j]);
    //             if (!element->is_reachable() && !element->inserted) {
    //                 new_root.push_back(element);
    //                 element->inserted = true;
    //             }
    //         }
    //     }
    // }
}
