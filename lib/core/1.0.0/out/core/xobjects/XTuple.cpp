//
// Created by chris on 4/3/21.
//

#include "XTuple.h"
#include "../basics.h"

TaggedObject* core_D_Tuple2::__eq__(TaggedObject* pObject) {
    core_D_Tuple2* other = CAST(pObject, core_D_Tuple2);
    return MAKE_BOOL(GET_BOOL(EQ(this->mem_1, other->mem_1)) && GET_BOOL(EQ(this->mem_2, other->mem_2)));
}

core_D_Tuple2::~core_D_Tuple2() {
    GC::out_of_scope(this->mem_1);
    GC::out_of_scope(this->mem_2);
}

core_D_Tuple2::core_D_Tuple2(TaggedObject* mem_1, TaggedObject* mem_2) : XObject("Tuple2") {
    this->mem_1 = GC::declare(mem_1);
    this->mem_2 = GC::declare(mem_2);
}

TaggedObject* core_D_Tuple3::__eq__(TaggedObject* pObject) {
    core_D_Tuple3* other = CAST(pObject, core_D_Tuple3);
    return MAKE_BOOL(GET_BOOL(EQ(this->mem_1, other->mem_1)) && GET_BOOL(EQ(this->mem_2, other->mem_2)) &&
                     GET_BOOL(EQ(this->mem_3, other->mem_3)));
}

core_D_Tuple3::~core_D_Tuple3() {
    GC::out_of_scope(mem_1);
    GC::out_of_scope(mem_2);
    GC::out_of_scope(mem_3);
}

core_D_Tuple3::core_D_Tuple3(TaggedObject* mem_1, TaggedObject* mem_2, TaggedObject* mem_3) : XObject("Tuple3") {
    this->mem_1 = GC::declare(mem_1);
    this->mem_2 = GC::declare(mem_2);
    this->mem_3 = GC::declare(mem_3);
}

