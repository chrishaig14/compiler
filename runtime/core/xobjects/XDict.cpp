//
// Created by chris on 3/3/21.
//

#include "XDict.h"
#include "../GC.h"


XDict::XDict(std::unordered_map<TaggedObject*, TaggedObject*> v) : XObject("Dict") {
    for (auto& it: v) {
        this->l[hash(it.first)] = it.second;
        if (has_tag(it.second, OBJECT_TAG)) {
            UNTAG(it.second)->inc_count();
        }
    }

    this->is_list = false;
}

XDict::~XDict() {
    if (has_tag(this->l.begin()->second, OBJECT_TAG)) {
        for (auto& it: this->l) {
            GC::out_of_scope(it.second);
        }
    }
}

