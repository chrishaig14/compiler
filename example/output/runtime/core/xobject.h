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

#define TRUE_TAG 0b001
#define FALSE_TAG 0b010
#define INT_TAG 0b011
#define OBJECT_TAG 0b100
#define FUNCTION_TAG 0b101
#define CAST(ptr, cls) ((cls*)UNTAG(ptr))

extern TaggedObject* TRUE;
extern TaggedObject* FALSE;


inline XObject* UNTAG(TaggedObject* l) {
    return (XObject*) ((unsigned long) l & 0xfffffffffffffff8);
}

inline TaggedObject* TAG(XObject* s) {
    return (TaggedObject*) ((unsigned long) s | OBJECT_TAG);
}

inline TaggedObject* FTAG(XObject* s) {
    return (TaggedObject*) ((unsigned long) s | FUNCTION_TAG);
}

inline bool has_tag(TaggedObject* p, unsigned long tag) {
    return ((unsigned long) p & 0b111) == tag;
}

inline TaggedObject* MAKE_INT(long x) {
    return (TaggedObject*) ((unsigned long) (x << 3) | INT_TAG);
}

inline TaggedObject* MAKE_BOOL(bool x) {
    return (TaggedObject*) (unsigned long) (x ? TRUE_TAG : FALSE_TAG);
}

inline bool GET_BOOL(TaggedObject* x) {
    return (unsigned long) x == TRUE_TAG;
}


inline long GET_INT(TaggedObject* x) {
    return (long) x >> 3;
};


#endif //UNTITLED1_XOBJECT_H