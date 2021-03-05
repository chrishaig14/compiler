//
// Created by chris on 4/3/21.
//

#ifndef XLANG_XTUPLE_H
#define XLANG_XTUPLE_H

#include "XObject.h"
#include "../xobject.h"
#include <assert.h>

class XTuple : public XObject {
public:

    std::vector<TaggedObject*> members;

    XTuple(const std::string& n, int num);

    virtual TaggedObject* get_member(int i) = 0;

    virtual void mark(std::vector<XObject*>& new_root) override;
};

class XTuple2 : public XTuple {
public:

    XTuple2(TaggedObject* mem_1, TaggedObject* mem_2);
    TaggedObject* get_member(int i) override;
};

class XTuple3 : public XTuple {
public:

    XTuple3(TaggedObject* mem_1, TaggedObject* mem_2, TaggedObject* mem_3);

    TaggedObject* get_member(int i) override;
};


#endif //XLANG_XTUPLE_H
