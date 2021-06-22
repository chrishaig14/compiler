//
// Created by chris on 3/3/21.
//

#ifndef XLANG_XLIST_H
#define XLANG_XLIST_H

#include "XObject.h"
#include "../Function.h"
#include "../macros.h"
#include "../GC.h"


struct XList {
    std::vector<TaggedObject*> lv;
    Vtable* vtable;
};


TaggedObject* XList_init_with_length(size_t n);

extern TaggedObject* core_D_core_D_List_D_len;
extern TaggedObject* core_D_core_D_List_D_has;
extern TaggedObject* core_D_core_D_List_D_map;
extern TaggedObject* core_D_core_D_List_D_push;
extern TaggedObject* core_D_core_D_List_D_where;
extern TaggedObject* core_D_core_D_List_D___get_item__;
extern TaggedObject* core_D_core_D_List_D___set_item__;

extern TaggedObject* core_D_core_D_List_D___add__;

extern Function2 core_D_core_D_List_D_has_o;
extern Function1 core_D_core_D_List_D_len_o;
extern Function2 core_D_core_D_List_D_map_o;
extern Function2 core_D_core_D_List_D_push_o;
extern Function2 core_D_core_D_List_D_where_o;
extern Function2 core_D_core_D_List_D___get_item__o;
extern Function2 core_D_core_D_List_D___set_item__o;

extern Function2 core_D_core_D_List_D___add__o;

TaggedObject* core_D_core_D_List_D_has_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_List_D_map_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_List_D_push_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_List_D_where_f(TaggedObject* a, TaggedObject* b);

TaggedObject* core_D_core_D_List_D_len_f(TaggedObject* a);
TaggedObject* core_D_core_D_List_D___get_item___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_core_D_List_D___set_item___f(TaggedObject* a, TaggedObject* b, TaggedObject* c);

TaggedObject* core_D_core_D_List_D___add___f(TaggedObject* a, TaggedObject* b);

#endif //XLANG_XLIST_H
