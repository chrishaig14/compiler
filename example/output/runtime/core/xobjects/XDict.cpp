//
// Created by chris on 3/3/21.
//

#include "XDict.h"


XDict::XDict(std::unordered_map<TaggedObject*, TaggedObject*> v) : XObject("Dict") {
    for (auto& it: v) {
        this->l[hash(it.first)] = it.second;
    }
    this->is_list = false;
}

