//
// Created by chris on 25/4/21.
//

#ifndef XLANG_BOOLEAN_H
#define XLANG_BOOLEAN_H

#include "../Function.h"

extern Function1 core_D_Boolean_D___not___o;
extern Function1 core_D_Boolean_D_str_o;
extern Function2 core_D_Boolean_D___eq___o;
extern Function2 core_D_Boolean_D___ne___o;
extern Function2 core_D_Boolean_D___or___o;
extern Function2 core_D_Boolean_D___and___o;

extern TaggedObject* core_D_Boolean_D___eq__;
extern TaggedObject* core_D_Boolean_D___ne__;
extern TaggedObject* core_D_Boolean_D___or__;
extern TaggedObject* core_D_Boolean_D___and__;
extern TaggedObject* core_D_Boolean_D_str;
extern TaggedObject* core_D_Boolean_D___not__;

TaggedObject* core_D_Boolean_D___eq___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Boolean_D_str_f(TaggedObject* _i);
TaggedObject* core_D_Boolean_D___not___f(TaggedObject* _i);
TaggedObject* core_D_Boolean_D___ne___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Boolean_D___and___f(TaggedObject* a, TaggedObject* b);
TaggedObject* core_D_Boolean_D___or___f(TaggedObject* a, TaggedObject* b);

#endif //XLANG_BOOLEAN_H
