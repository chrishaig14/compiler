//
// Created by chris on 17/12/20.
//

#include "xobject.h"
#include "core.h"

int hash(XObject* n) {
    if (has_tag(n, INT_TAG)) {
        return PTR_TO_INT(n);
    }
    Tuple2* t = dynamic_cast<Tuple2*>(UNTAG(n));
    int h = 0;

    if (t != nullptr) {
        h = hash(t->get_member(1));
        h += hash(t->get_member(2));
    } else {
        XString* s = PTR_TO_STRING(n);
        const std::string& str = s->s;
        for (int i = 0; i < str.size(); i++) {
            h += int(str[i]);
        }
    }
    return h;
}

XObject* f_File_read_line(XObject* o) {
    XFile* f = (XFile*) UNTAG(o);
    std::string line;
    std::getline(f->f, line);
    return new XString(line);
}

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
