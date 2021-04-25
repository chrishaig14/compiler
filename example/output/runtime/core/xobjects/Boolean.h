//
// Created by chris on 25/4/21.
//

#ifndef XLANG_BOOLEAN_H
#define XLANG_BOOLEAN_H

#include "../Function.h"

extern Function1 core_D_Boolean_D_str_o;
extern Function2 core_D_Boolean_D_eq_o;
extern Function2 core_D_Boolean_D_ne_o;
extern Function2 core_D_Boolean_D_or_o;
extern Function2 core_D_Boolean_D_and_o;

extern TaggedObject* core_D_Boolean_D_eq;
extern TaggedObject* core_D_Boolean_D_ne;
extern TaggedObject* core_D_Boolean_D_or;
extern TaggedObject* core_D_Boolean_D_and;
extern TaggedObject* core_D_Boolean_D_str;

TaggedObject* core_D_Boolean_D_eq_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Boolean_D_str_f(TaggedObject* _i);
TaggedObject* core_D_Boolean_D_ne_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Boolean_D_and_f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Boolean_D_or_f(TaggedObject* a, TaggedObject* b);

#endif //XLANG_BOOLEAN_H
