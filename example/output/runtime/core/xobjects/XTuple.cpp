//
// Created by chris on 4/3/21.
//

#include "XTuple.h"
#include "../basics.h"

TaggedObject* XTuple2::__eq__(TaggedObject* pObject) {
    XTuple2* other = CAST(pObject, XTuple2);
    return MAKE_BOOL(GET_BOOL(EQ(this->mem_1, other->mem_1)) && GET_BOOL(EQ(this->mem_2, other->mem_2)));
}

XTuple2::~XTuple2() {
    GC::out_of_scope(this->mem_1);
    GC::out_of_scope(this->mem_2);
}

XTuple2::XTuple2(TaggedObject* mem_1, TaggedObject* mem_2) : XObject("Tuple2") {
    this->mem_1 = GC::declare(mem_1);
    this->mem_2 = GC::declare(mem_2);
}

TaggedObject* XTuple3::__eq__(TaggedObject* pObject) {
    XTuple3* other = CAST(pObject, XTuple3);
    return MAKE_BOOL(GET_BOOL(EQ(this->mem_1, other->mem_1)) && GET_BOOL(EQ(this->mem_2, other->mem_2)) &&
                     GET_BOOL(EQ(this->mem_3, other->mem_3)));
}

XTuple3::~XTuple3() {
    GC::out_of_scope(mem_1);
    GC::out_of_scope(mem_2);
    GC::out_of_scope(mem_3);
}

XTuple3::XTuple3(TaggedObject* mem_1, TaggedObject* mem_2, TaggedObject* mem_3) : XObject("Tuple3") {
    this->mem_1 = GC::declare(mem_1);
    this->mem_2 = GC::declare(mem_2);
    this->mem_3 = GC::declare(mem_3);
}

