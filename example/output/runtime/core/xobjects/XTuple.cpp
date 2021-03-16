//
// Created by chris on 4/3/21.
//

#include "XTuple.h"
#include "../basics.h"

XTuple::XTuple(const std::string& n, int num) : XObject(n), members(num, nullptr) {
}

XTuple2::XTuple2(TaggedObject* mem_1, TaggedObject* mem_2) : XTuple("Tuple2", 2) {
    if (has_tag(mem_1, OBJECT_TAG)) {
        UNTAG(mem_1)->inc_count();
    }
    if (has_tag(mem_2, OBJECT_TAG)) {
        UNTAG(mem_2)->inc_count();
    }
    this->mem_1 = mem_1;
    this->mem_2 = mem_2;
}

TaggedObject* XTuple2::__eq__(TaggedObject* pObject) {
    XTuple3* other = CAST(pObject, XTuple3);
    if (EQ(this->mem_1, other->mem_1) == MAKE_BOOL(false)) {
        return MAKE_BOOL(false);
    }
    if (EQ(this->mem_2, other->mem_2) == MAKE_BOOL(false)) {
        return MAKE_BOOL(false);
    }
    return MAKE_BOOL(true);
}

XTuple2::~XTuple2() {
    if (has_tag(this->mem_1, OBJECT_TAG)) {
        UNTAG(this->mem_1)->dec_count();
        if (UNTAG(this->mem_1)->count == 0) {
            delete UNTAG(this->mem_1);
        }
    }
    if (has_tag(this->mem_2, OBJECT_TAG)) {
        UNTAG(this->mem_2)->dec_count();
        if (UNTAG(this->mem_2)->count == 0) {
            delete UNTAG(this->mem_2);
        }
    }
}

XTuple3::XTuple3(TaggedObject* mem_1, TaggedObject* mem_2, TaggedObject* mem_3) : XTuple("Tuple3", 3) {
    this->mem_1 = mem_1;
    this->mem_2 = mem_2;
    this->mem_3 = mem_3;
}

TaggedObject* XTuple3::__eq__(TaggedObject* pObject) {
    XTuple3* other = CAST(pObject, XTuple3);
    if (EQ(this->mem_1, other->mem_1) == MAKE_BOOL(false)) {
        return MAKE_BOOL(false);
    }
    if (EQ(this->mem_2, other->mem_2) == MAKE_BOOL(false)) {
        return MAKE_BOOL(false);
    }
    if (EQ(this->mem_3, other->mem_3) == MAKE_BOOL(false)) {
        return MAKE_BOOL(false);
    }
    return MAKE_BOOL(true);
}

