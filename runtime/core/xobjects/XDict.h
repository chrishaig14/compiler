//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XDICT_H
#define XLANG_XDICT_H


#include <unordered_map>
#include "XObject.h"
#include "../Function.h"
#include "../../macros.h"
#include "../GC.h"


int hash(TaggedObject* n);

class XDict : public XObject {
public:
    std::unordered_map<int, TaggedObject*> l;

    XDict(std::unordered_map<TaggedObject*, TaggedObject*> v);
    ~XDict() override;

};


extern TaggedObject* core_D_Dict_D_len;
extern TaggedObject* core_D_Dict_D_has;
extern TaggedObject* core_D_Dict_D_push;
extern TaggedObject* core_D_Dict_D___get_item__;
extern TaggedObject* core_D_Dict_D___set_item__;

extern Function1 core_D_Dict_D_len_o;
extern Function2 core_D_Dict_D_has_o;
extern Function2 core_D_Dict_D_push_o;
extern Function2 core_D_Dict_D___get_item__o;
extern Function3 core_D_Dict_D___set_item__o;

TaggedObject* core_D_Dict_D_has_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Dict_D_push_f(TaggedObject* a, TaggedObject* b);

TaggedObject* core_D_Dict_D_len_f(TaggedObject* a);
TaggedObject* core_D_Dict_D___get_item___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Dict_D___set_item___f(TaggedObject* a, TaggedObject* k, TaggedObject* v);


#endif //XLANG_XDICT_H
