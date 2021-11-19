//
// Created by chris on 25/4/21.
//

#ifndef XLANG_FUNCTIONS_H
#define XLANG_FUNCTIONS_H

#include "core.h"

extern TaggedObject* core_D_open;
extern TaggedObject* core_D_core_D_print;
extern TaggedObject* core_D_exit;
extern TaggedObject* core_D_core_D_range;
extern TaggedObject* core_D_map;
extern TaggedObject* core_D_join;

extern Function1 core_D_open_o;
extern Function1 core_D_core_D_print_o;
extern Function1 core_D_exit_o;
extern Function3 core_D_core_D_range_o;
extern Function2 core_D_map_o;
extern Function2 core_D_join_o;

TaggedObject* core_D_exit_f(TaggedObject* c);
TaggedObject* core_D_core_D_print_f(TaggedObject* _s);
TaggedObject* core_D_core_D_range_f(TaggedObject* _start, TaggedObject* _step, TaggedObject* _end);
TaggedObject* core_D_map_f(TaggedObject* _l, TaggedObject* _f);
TaggedObject* core_D_join_f(TaggedObject* _l, TaggedObject* _s);
TaggedObject* core_D_open_f(TaggedObject* _s);

#endif //XLANG_FUNCTIONS_H
