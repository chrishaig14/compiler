//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XDICT_H
#define XLANG_XDICT_H


#include <unordered_map>
#include "XObject.h"

int hash(XObject* n);

class XDict : public XObject {
public:
    std::unordered_map<int, XObject*> l;

    XDict(std::unordered_map<XObject*, XObject*> v);

    void mark(std::vector<XObject*>& new_root) override;
};


#endif //XLANG_XDICT_H
