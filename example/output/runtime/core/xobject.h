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
#define CAST(ptr, cls) ((cls*)UNTAG(ptr))

extern TaggedObject* TRUE;
extern TaggedObject* FALSE;


inline XObject* UNTAG(TaggedObject* l) {
    return (XObject*) ((unsigned long) l & 0xfffffffffffffff0);
}

inline TaggedObject* TAG(XObject* s) { return (TaggedObject*) ((unsigned long) s | OBJECT_TAG); }

inline bool has_tag(TaggedObject* p, unsigned long tag) {
    return ((unsigned long) p & 0b1110) == tag;
}

inline TaggedObject* MAKE_INT(long x) { return (TaggedObject*) ((unsigned long) (x << 4) | INT_TAG); }

inline TaggedObject* MAKE_BOOL(bool x) { return (TaggedObject*) (unsigned long) (x ? TRUE_TAG : FALSE_TAG); }

inline bool GET_BOOL(TaggedObject* x) { return (unsigned long) x == TRUE_TAG; }


inline long GET_INT(TaggedObject* x) { return (long) x >> 4; };


#endif //UNTITLED1_XOBJECT_H