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
#define FLOAT_TAG 0b110
#define EXCEPTION_TAG 0b111
#define CAST(ptr, cls) ((cls*)UNTAG(ptr))

extern TaggedObject* TRUE;
extern TaggedObject* FALSE;

struct Vtable {
    void (* clean)(TaggedObject*);
    std::vector<XObject*> (* get_all_members)(TaggedObject*);
    TaggedObject* (* str)(TaggedObject*);
};

inline XObject* UNTAG(TaggedObject* l) {
    return (XObject*) ((unsigned long) l & 0xfffffffffffffff8);
}

inline TaggedObject* TAG(void* s) {
    return (TaggedObject*) ((unsigned long) s | OBJECT_TAG);
}

inline TaggedObject* FTAG(XObject* s) {
    return (TaggedObject*) ((unsigned long) s | FUNCTION_TAG);
}

inline bool has_tag(TaggedObject* p, unsigned long tag) {
    return ((unsigned long) p & 0b111) == tag;
}

inline TaggedObject* set_tag(TaggedObject* p, unsigned long tag) {
    return (TaggedObject*) ((unsigned long) p | 0b111);
}

inline TaggedObject* MAKE_FLOAT(float x) {
    return (TaggedObject*) ((*(unsigned long*) &x) << 32 | FLOAT_TAG);
}

inline float GET_FLOAT(TaggedObject* x) {
    unsigned long p = (unsigned long) x >> 32;
    return *(float*) &p;
}

inline TaggedObject* MAKE_INT(long x) {
    return (TaggedObject*) ((unsigned long) x << 3 | INT_TAG);
}

inline TaggedObject* MAKE_BOOL(bool x) {
    return (TaggedObject*) (unsigned long) (x ? TRUE_TAG : FALSE_TAG);
}

inline bool GET_BOOL(TaggedObject* x) {
    return (unsigned long) x == TRUE_TAG;
}


inline long GET_INT(TaggedObject* x) {
    return (long) x >> 3;
}


#endif //UNTITLED1_XOBJECT_H