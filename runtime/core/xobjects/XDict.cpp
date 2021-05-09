//
// Created by chris on 3/3/21.
//

#include "XDict.h"
#include "../GC.h"

// DEFINE_FUNCTION(1, core_D_Dict_D_len)
// DEFINE_FUNCTION(2, core_D_Dict_D_has)
// DEFINE_FUNCTION(2, core_D_Dict_D_push)
DEFINE_FUNCTION(2, core_D_Dict_D___get_item__)
DEFINE_FUNCTION(3, core_D_Dict_D___set_item__)

XDict::XDict(std::unordered_map<TaggedObject*, TaggedObject*> v) : XObject("Dict") {
    for (auto& it: v) {
        int h = hash(it.first);
        this->l[h] = it.second;
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


TaggedObject* core_D_Dict_D___get_item___f(TaggedObject* a, TaggedObject* b) {
    int h = hash(b);
    auto& l = (CAST(a, XDict)->l);
    if (l.count(h) == 0) {
        throw std::runtime_error("KEY NOT IN DICT!");
    }
    return l.at(h);
}

TaggedObject* core_D_Dict_D___set_item___f(TaggedObject* a, TaggedObject* k, TaggedObject* v) {
    (CAST(a, XDict)->l)[hash(k)] = v;
    GC::declare(k);
    GC::declare(v);
    return nullptr;
}