//
// Created by chris on 4/3/21.
//

#ifndef XLANG_XTUPLE_H
#define XLANG_XTUPLE_H

#include "XObject.h"
#include "../xobject.h"
#include <assert.h>

class XTuple2 : public XObject {
public:
    TaggedObject* mem_1;
    TaggedObject* mem_2;
    TaggedObject* __eq__(TaggedObject* pObject) override;
    ~XTuple2() override;

    XTuple2(TaggedObject* mem_1, TaggedObject* mem_2);
};

class XTuple3 : public XObject {
public:
    TaggedObject* mem_1;
    TaggedObject* mem_2;
    TaggedObject* mem_3;
    TaggedObject* __eq__(TaggedObject* pObject) override;
    ~XTuple3() override;

    XTuple3(TaggedObject* mem_1, TaggedObject* mem_2, TaggedObject* mem_3);
};


#endif //XLANG_XTUPLE_H
