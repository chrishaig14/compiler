//
// Created by chris on 4/3/21.
//

#include "XTuple.h"

XTuple::XTuple(const std::string& n, int num) : XObject(n), members(num, nullptr) {}

void XTuple::mark(std::vector<XObject*>& new_root) {
    for (auto& m: this->members) {
        if (has_tag(m, OBJECT_TAG)) {
            XObject* element = UNTAG(m);
            if (!element->is_reachable() && !element->inserted) {
                new_root.push_back(element);
                element->inserted = true;
            }
        }
    }
}


XTuple2::XTuple2(TaggedObject* mem_1, TaggedObject* mem_2) : XTuple("Tuple2", 2) {
    this->members[0] = mem_1;
    this->members[1] = mem_2;
}

TaggedObject* XTuple2::get_member(int i) {
    assert(i >= 1 && i <= 2);
    return this->members[i - 1];
}

XTuple3::XTuple3(TaggedObject* mem_1, TaggedObject* mem_2, TaggedObject* mem_3) : XTuple("Tuple3", 3) {
    this->members[0] = mem_1;
    this->members[1] = mem_2;
    this->members[2] = mem_3;
}

TaggedObject* XTuple3::get_member(int i) {
    assert(i >= 1 && i <= 3);
    return this->members[i - 1];
}
