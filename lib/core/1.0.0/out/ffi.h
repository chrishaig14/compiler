//
// Created by chris on 19/5/21.
//

#ifndef XLANG_FFI_H
#define XLANG_FFI_H

#include "core.h"

#define FUN1(name, arg0) TaggedObject* name_f(TaggedObject* arg0)
#define DEF_FUN2(name, arg0, arg1) TaggedObject* name##_f(TaggedObject* arg0, TaggedObject* arg1)
#define DECL_FUN2(name, arg0, arg1) extern TaggedObject* name;extern Function2 name##_o; DEF_FUN2(name, arg0, arg1)

DECL_FUN2(core_D_add_and_square, x, y);

#endif //XLANG_FFI_H
