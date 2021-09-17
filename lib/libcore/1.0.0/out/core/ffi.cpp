//
// Created by chris on 19/5/21.
//

#include "ffi.h"

DEFINE_FUNCTION(2, core_D_add_and_square)

DEF_FUN2(core_D_add_and_square, x, y) {
    TaggedObject* r = MAKE_INT(GET_INT(x) + GET_INT(y));
    return MAKE_INT(GET_INT(r) * GET_INT(r));
}