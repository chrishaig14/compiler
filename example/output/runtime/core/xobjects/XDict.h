//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XDICT_H
#define XLANG_XDICT_H


#include <unordered_map>
#include "XObject.h"

int hash(TaggedObject* n);

class XDict : public XObject {
public:
    std::unordered_map<int, TaggedObject*> l;

    XDict(std::unordered_map<TaggedObject*, TaggedObject*> v);

    void mark(std::vector<XObject*>& new_root) override;
};


#endif //XLANG_XDICT_H
