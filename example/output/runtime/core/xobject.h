//
// Created by chris on 17/12/20.
//

#ifndef UNTITLED1_XOBJECT_H
#define UNTITLED1_XOBJECT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "xobjects/XObject.h"

#define TRUE_TAG 0b0010
#define FALSE_TAG 0b0100
#define INT_TAG 0b0110
#define OBJECT_TAG 0b1100
#define REACHABLE_FLAG 0b0001
#define CAST(ptr, cls) ((cls*)UNTAG(ptr))


inline XObject* UNTAG(XObject* l) {
    return (XObject*) ((unsigned long) l & 0xfffffffffffffff0);
}


inline XObject* PTR_TO_OBJ(XObject* x) {
    return (XObject*) ((unsigned long) x & 0xfffffffffffffff0);
}

inline XObject* TAG(XObject* s) { return (XObject*) ((unsigned long) s | OBJECT_TAG); }

inline bool has_tag(XObject* p, unsigned long tag) {
    return ((unsigned long) p & 0b1110) == tag;
}

inline bool is_reachable(XObject* p) {
    return ((unsigned long) p & 0b1);
}

inline void set_reachable(XObject** p) {
    *p = (XObject*) ((unsigned long) *p | 0b1);
}


inline XObject* INT_TO_PTR(long x) { return (XObject*) ((unsigned long) (x << 4) | INT_TAG); }

inline XObject* BOOL_TO_PTR(bool x) { return (XObject*) (unsigned long) (x ? TRUE_TAG : FALSE_TAG); }

inline bool PTR_TO_BOOL(XObject* x) { return (unsigned long) x == TRUE_TAG; }


inline long PTR_TO_INT(XObject* x) { return (long) x >> 4; };




XObject* f_File_read_line(XObject* o);

#endif //UNTITLED1_XOBJECT_H